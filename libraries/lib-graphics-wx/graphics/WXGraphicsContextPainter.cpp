/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXGraphicsContextPainter.cpp

  Dmitry Vedenko

**********************************************************************/
#include "WXGraphicsContextPainter.h"

#include <cstring>
#include <unordered_map>

#include <wx/graphics.h>
#include <wx/region.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/window.h>
#include <wx/dcbuffer.h>

#include "graphics/Renderer.h"
#include "graphics/RendererID.h"

#include "WXPainterUtils.h"
#include "WXFontUtils.h"
#include "WXColor.h"

namespace graphics::wx
{

namespace
{
struct WXGraphicsRendererShutdownMessage : Observer::Message
{
};

struct WXGraphicsRendererShutdownPublisher :
    Observer::Publisher<WXGraphicsRendererShutdownMessage>
{
   using Publisher::Publish;
};

WXGraphicsRendererShutdownPublisher& GetWXGraphicsRendererShutdownPublisher()
{
   static WXGraphicsRendererShutdownPublisher publisher;
   return publisher;
}

//! Implementations of the graphics::Renderer interface for wxGraphicsRenderer
class WXGraphicsContextRenderer : public Renderer
{
public:
   ~WXGraphicsContextRenderer()
   {
   }
   
   RendererID GetRendererID() const noexcept override;

   bool IsAvailable() const noexcept override
   {
      return true;
   }

   // This is a hack that forces the factory to pass wxWindow* into CreateWindowPainter,
   // as the implementation relies heavily on wxAutoBufferedPaintDC
   bool ExpectsNativeHandle() const noexcept override
   {
      return false;
   }

   void Shutdown() override
   {
      GetWXGraphicsRendererShutdownPublisher().Publish({});
   }

   wxGraphicsRenderer* GetWXRenderer() const noexcept
   {
      return wxGraphicsRenderer::GetDefaultRenderer();
   }

   std::unique_ptr<Painter> CreateWindowPainter(void* window, const FontInfo& defaultFont) override
   {
      return std::make_unique<WXGraphicsContextPainter>(
         GetWXRenderer(), static_cast<wxWindow*>(window),
         wxFontFromFontInfo(defaultFont));
   }

   std::unique_ptr<Painter>
   CreateMeasuringPainter(const FontInfo& defaultFont) override
   {
      return std::make_unique<WXGraphicsContextPainter>(
         GetWXRenderer(), wxFontFromFontInfo(defaultFont));
   }

   std::unique_ptr<Painter>
   CreateOffscreenPainter(const FontInfo& defaultFont) override
   {
      return std::make_unique<WXGraphicsContextPainter>(
         GetWXRenderer(), wxFontFromFontInfo(defaultFont));      
   }
}; // class WXGraphicsContextRenderer

auto rendererId = RegisterRenderer(
   RendererPriority::Fallback, "Fallback",
   []() { return std::make_unique<WXGraphicsContextRenderer>(); });

RendererID WXGraphicsContextRenderer::GetRendererID() const noexcept
{
   return rendererId;
}

// PainterImage implementation
struct wxGraphicsContextPainterImage final : public PainterImage
{
   wxGraphicsContextPainterImage(
      Painter& painter, wxGraphicsRenderer& renderer, wxImage& img)
       : PainterImage(painter)
       , Bitmap(renderer.CreateBitmapFromImage(img))
       , Width(img.GetWidth())
       , Height(img.GetHeight())
       , HasAlpha(img.HasAlpha())
   {
      RendererShutdownSubscription =
         GetWXGraphicsRendererShutdownPublisher().Subscribe([this](auto)
                                                            { Bitmap = {}; });
   }

   wxGraphicsContextPainterImage(
      Painter& painter, const wxGraphicsContextPainterImage& rhs, uint32_t x,
      uint32_t y, uint32_t width, uint32_t height)
       : PainterImage(painter)
       , Bitmap(rhs.Bitmap.GetRenderer()->CreateSubBitmap(
            rhs.Bitmap, x, y, width, height))
       , Width(width)
       , Height(height)
       , HasAlpha(rhs.HasAlpha)
   {
      RendererShutdownSubscription =
         GetWXGraphicsRendererShutdownPublisher().Subscribe([this](auto)
                                                            { Bitmap = {}; });
   }

   std::vector<uint8_t> GetData() const override
   {
      if (Bitmap.IsNull())
         return {};

      auto image = Bitmap.ConvertToImage();

      std::vector<uint8_t> data;

      if (HasAlpha)
      {
         data.reserve(Width * Height * 4);
         
         for (uint32_t row = 0; row < Height; ++row)
         {
            for (uint32_t col = 0; col < Width; ++col)
            {
               const size_t alphaOffset = row * image.GetWidth() + col;
               const size_t rgbOffset = 3 * alphaOffset;
               
               data.push_back(image.GetData()[rgbOffset]);
               data.push_back(image.GetData()[rgbOffset + 1]);
               data.push_back(image.GetData()[rgbOffset + 2]);
               data.push_back(image.GetAlpha()[alphaOffset]);
            }
         }
      }
      else
      {
         data.reserve(Width * Height * 3);

         for (uint32_t row = 0; row < Height; ++row)
         {
            for (uint32_t col = 0; col < Width; ++col)
            {
               const size_t rgbOffset = 3 * (row * image.GetWidth() + col);

               data.push_back(image.GetData()[rgbOffset]);
               data.push_back(image.GetData()[rgbOffset + 1]);
               data.push_back(image.GetData()[rgbOffset + 2]);
            }
         }
      }

      return data;
   }

   wxGraphicsBitmap Bitmap;

   uint32_t Width;
   uint32_t Height;

   bool HasAlpha;

   uint32_t GetWidth() const override
   {
      return Width;
   }

   uint32_t GetHeight() const override
   {
      return Height;
   }

   Observer::Subscription RendererShutdownSubscription;
}; // struct wxGraphicsContextPainterImage

// PainterFont implementation
// wxGraphicsFont encapsulates the text color,
// so we have to store wxFont and to cache wxGraphicsFont for the
// last used context
struct wxGraphicsContextPainterFont final : public PainterFont
{
   wxGraphicsContextPainterFont(
      Painter& painter, const wxFont& font)
       : PainterFont(painter)
       , Font(font)
       , FaceName(font.GetFaceName().ToUTF8().data())
       , FontSize(font.GetPixelSize().y)
   {
      // wxGraphicsContext does not allow to get font metrics,
      // so we have to create a temporary DC
      wxMemoryDC dc;

      dc.SetFont(font);
      const auto dcMetrics = dc.GetFontMetrics();

      FontMetrics.Ascent = dcMetrics.ascent;
      FontMetrics.Descent = dcMetrics.descent;
      FontMetrics.Linegap = dcMetrics.externalLeading;
      FontMetrics.LineHeight = dcMetrics.height;

      // Destroys all the resources associated with the font
      // because wxWidgets will destroy all wxGraphicsRenderer resources
      // before all the Audacity UI objects are destroyed
      RendererShutdownSubscription =
         GetWXGraphicsRendererShutdownPublisher().Subscribe([this](auto)
            {
               GraphicsFonts.clear();
               MeasuringContext.reset();
               Font = {};
            });
   }

   std::string_view GetFace() const override
   {
      return FaceName;
   }

   float GetFontSize() const override
   {
      throw FontSize;
   }

   Metrics GetFontMetrics() const override
   {
      return FontMetrics;
   }

   Size GetTextSize(
      const std::string_view& text, bool) const override
   {
      return GetTextSize(nullptr, text);
   }

   // Measuring text is not trivial under with wxGraphicsContext.
   // If the context is not provided - we create a new measuring context
   // and use it to measure the text.
   Size GetTextSize(wxGraphicsContext* ctx, const std::string_view& text) const
   {
      if (ctx == nullptr)
      {
         MeasuringContext.reset(wxGraphicsContext::Create());
         ctx = MeasuringContext.get();
      }

      const auto& graphicsFont = GetGraphicsFont(*ctx, Colors::Black);

      ctx->SetFont(graphicsFont);

      double width, height;

      ctx->GetTextExtent(
         wxString::FromUTF8(text.data(), text.size()), &width, &height);

      return { static_cast<float>(width), static_cast<float>(height) };
   }

   // Gets the cached font for the given context with a given color.
   // Creates font if the request cannot be satisfied from cache.
   // Cache is invalidated if a new context is provided. There is no
   // sense to maintain per context cache, because contexts are short lived
   const wxGraphicsFont& GetGraphicsFont(wxGraphicsContext& ctx, Color color) const
   {
      if (CacheContext != &ctx)
      {
         CacheContext = &ctx;
         GraphicsFonts.clear();
      }

      auto it = GraphicsFonts.find(color.GetABGR());

      if (it != GraphicsFonts.end())
         return it->second;

      return GraphicsFonts.emplace(std::make_pair(
         color.GetABGR(), ctx.CreateFont(Font, wxColorFromColor(color)))).first->second;
   }

   wxFont Font;

   mutable std::unique_ptr<wxGraphicsContext> MeasuringContext;

   mutable wxGraphicsContext* CacheContext { nullptr };
   mutable std::unordered_map<uint32_t, wxGraphicsFont> GraphicsFonts;

   std::string FaceName;
   float FontSize;

   Metrics FontMetrics;

   Observer::Subscription RendererShutdownSubscription;
}; // struct wxGraphicsContextPainterFont

// PainterPath implementation
class wxGraphicsContextPainterPath final : public PainterPath
{
public:
   explicit wxGraphicsContextPainterPath(
      Painter& painter, wxGraphicsRenderer& renderer)
       : PainterPath(painter)
       , mPath(renderer.CreatePath())
   {
   }

   void EndFigure(bool closed) override
   {
      if (closed)
         mPath.CloseSubpath();
   }

   void DoLineTo(Point pt) override
   {
      mPath.AddLineToPoint(pt.x, pt.y);
   }

   void DoMoveTo(Point pt) override
   {
      mPath.MoveToPoint(pt.x, pt.y);
   }

   void DoAddRect(const Rect& rect) override
   {
      mPath.AddRectangle(
         rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
   }

   void DrawPath(wxGraphicsContext& context) const
   {
      context.FillPath(mPath);
      context.StrokePath(mPath);
   }

private:
   wxGraphicsPath mPath;
}; // struct wxGraphicsContextPainterPath
} // anonymous namespace

// Stack of the paint targets (and contexts)!
class WXGraphicsContextPainter::PaintTargetStack final
{
public:
   PaintTargetStack(wxGraphicsRenderer* renderer, wxWindow* window)
       : mRenderer(renderer)
       , mWindow(window)
   {
   }

   PaintTargetStack(const PaintTargetStack&) = delete;
   PaintTargetStack(PaintTargetStack&&) = delete;
   PaintTargetStack& operator=(const PaintTargetStack&) = delete;
   PaintTargetStack& operator=(PaintTargetStack&&) = delete;

   void Push(std::shared_ptr<wxGraphicsContextPainterImage> image)
   {
      mPaintTargetsStack.emplace_back(std::make_unique<Surface>(
         *mRenderer, image->Width, image->Height, image->HasAlpha));
   }

   void Pop(std::shared_ptr<wxGraphicsContextPainterImage> image)
   {
      auto& currentItem = mPaintTargetsStack.back();

      currentItem->GC = {};
      image->Bitmap = mRenderer->CreateBitmapFromImage(currentItem->Image);

      mPaintTargetsStack.pop_back();
   }

   // Handles the on-screen paint event start
   bool BeginPaint()
   {
      assert(mPaintTargetsStack.empty());

      if (!mPaintTargetsStack.empty())
         return false;

      if (mWindow)
      {
         mPaintTargetsStack.emplace_back(
            std::make_unique<Surface>(*mRenderer, mWindow));
      }

      return !mPaintTargetsStack.empty();
   }

   // Handles the on-screen paint event end
   void EndPaint()
   {
      // RAII is used to maintain the state,
      // so this situation should never be possible
      // if the client code is well formed
      assert(mPaintTargetsStack.size() == 1);
      // It is safe to continue in any state
      mPaintTargetsStack.clear();
   }

   bool InPaintEnvent() const
   {
      return !mPaintTargetsStack.empty();
   }

   wxGraphicsContext* GetCurrentContext() const
   {
      return !mPaintTargetsStack.empty() ? mPaintTargetsStack.back()->GC.get() :
                                           nullptr;
   }

   wxGraphicsRenderer& GetRenderer() noexcept
   {
      return *mRenderer;
   }

   Size GetSize() const noexcept
   {
      if (InPaintEnvent())
      {
         auto ctx = GetCurrentContext();

         wxDouble width, height;
         ctx->GetSize(&width, &height);

         // Cases were observed, where GetSize failed 
         // when rendering off-scree
         if (width == 0 && height == 0)
         {
            width = mPaintTargetsStack.back()->Image.GetWidth();
            height = mPaintTargetsStack.back()->Image.GetHeight();
         }

         return { static_cast<float>(width), static_cast<float>(height) };
      }
      else if (mWindow)
      {
         const auto size = mWindow->GetSize();
         
         return { static_cast<float>(size.GetWidth()),
                  static_cast<float>(size.GetHeight()) };
      }

      // We are outside the paint event with no window associated
      return {};
   }

   bool IsSamePen(const Pen& pen) const
   {
      return !mPaintTargetsStack.empty() &&
             mPaintTargetsStack.back()->CurrentPen == pen;
   }

   void SetPen(const Pen& pen)
   {
      if (!mPaintTargetsStack.empty())
         mPaintTargetsStack.back()->SetPen(pen);
   }

   void SetBrush(const Brush& brush)
   {
      if (!mPaintTargetsStack.empty())
         mPaintTargetsStack.back()->SetBrush(brush);
   }

   void SetFont(const std::shared_ptr<wxGraphicsContextPainterFont>& font)
   {
      if (!mPaintTargetsStack.empty())
         mPaintTargetsStack.back()->SetFont(font);
   }

   void UpdateFont()
   {
      if (!mPaintTargetsStack.empty())
         mPaintTargetsStack.back()->UpdateFont();
   }

   void SetTransform(const Transform& transform)
   {
      if (!mPaintTargetsStack.empty())
         mPaintTargetsStack.back()->SetTransform(transform);
   }

   void SetClipRect(const Rect& rect)
   {
      if (!mPaintTargetsStack.empty())
         mPaintTargetsStack.back()->SetClipRect(rect);
   }

   bool SetAntialiasing(bool enabled)
   {
      if (!mPaintTargetsStack.empty())
         return mPaintTargetsStack.back()->SetAntialiasing(enabled);

      return false;
   }

private:
   // Structure that encapsulates the surface to be painted on
   struct Surface final
   {
      // Creates window surface
      Surface(wxGraphicsRenderer& renderer, wxWindow* window)
          : BufferedDC(std::make_unique<wxAutoBufferedPaintDC>(window))
          , GC(renderer.CreateContext(*BufferedDC))
      {
         InitGCState();
      }

      // Creates off-screen surface
      Surface(wxGraphicsRenderer& renderer, uint32_t width, uint32_t height, bool alpha)
          : Image(width, height)
      {
         if (alpha)
         {
            // Create the alpha array
            Image.SetAlpha();

            std::memset(
               Image.GetAlpha(), 0, Image.GetWidth() * Image.GetHeight());
         }

         GC.reset(renderer.CreateContextFromImage(Image));

         InitGCState();
      }

      void InitGCState()
      {
         GC->SetPen(wxPenFromPen(CurrentPen));
         GC->SetBrush(wxBrushFromBrush(CurrentBrush));

         CurrentAntialiasing =
            GC->SetAntialiasMode(
               CurrentAntialiasing ? wxANTIALIAS_DEFAULT : wxANTIALIAS_NONE) &&
            CurrentAntialiasing;
      }

      void SetPen(const Pen& pen)
      {
         if (CurrentPen != pen)
         {
            CurrentPen = pen;
            GC->SetPen(wxPenFromPen(pen));
         }
      }

      void SetBrush(const Brush& brush)
      {
         if (CurrentBrush != brush)
         {
            CurrentBrush = brush;

            if (brush.GetStyle() == BrushStyle::LinearGradient)
            {
               auto gradient = brush.GetGradientData();

               if (gradient != nullptr)
               {
                  wxGraphicsGradientStops stops;

                  for (auto stop : gradient->stops)
                  {
                     stops.Add(
                        wxColour(
                           stop.color.GetRed(), stop.color.GetGreen(),
                           stop.color.GetBlue(), stop.color.GetAlpha()),
                        stop.position);
                  }

                  GC->SetBrush(GC->CreateLinearGradientBrush(
                     gradient->firstPoint.x, gradient->firstPoint.y,
                     gradient->secondPoint.x, gradient->secondPoint.y, stops));
               }
               else
               {
                  GC->SetBrush(wxBrush());
               }
            }
            else
            {
               GC->SetBrush(wxBrushFromBrush(brush));
            }
         }
      }

      void SetFont(const std::shared_ptr<wxGraphicsContextPainterFont>& font)
      {
         if (font == nullptr)
            return;

         if (CurrentFont == nullptr || font->Font != CurrentFont->Font)
         {
            CurrentFont = font;
            FontDirty = true;
         }
      }

      void UpdateFont()
      {
         if (CurrentFont == nullptr)
            return;

         if (FontDirty || CurrentTextColor != CurrentBrush.GetColor())
         {
            CurrentTextColor = CurrentBrush.GetColor();
            FontDirty = false;

            GC->SetFont(CurrentFont->GetGraphicsFont(*GC, CurrentTextColor));
         }
      }

      void SetTransform(const Transform& transform)
      {
         if (CurrentTransform != transform)
         {
            CurrentTransform = transform;

            const wxGraphicsMatrix mtx = GC->CreateMatrix(
               transform.GetScale().x, 0.0, 0.0, transform.GetScale().y,
               transform.GetTranslation().x, transform.GetTranslation().y);

            GC->SetTransform(mtx);
         }
      }

      void SetClipRect(const Rect& rect)
      {
         if (CurrentClipRect != rect)
         {
            CurrentClipRect = rect;

            GC->ResetClip();

            if (
               std::isfinite(rect.size.width) &&
               std::isfinite(rect.size.height))
            {
               GC->Clip(
                  rect.origin.x, rect.origin.y, rect.size.width,
                  rect.size.height);
            }
         }
      }

      bool SetAntialiasing(bool enabled)
      {
         if (CurrentAntialiasing != enabled)
         {
            const bool supported = GC->SetAntialiasMode(
               CurrentAntialiasing ? wxANTIALIAS_DEFAULT : wxANTIALIAS_NONE);

            CurrentAntialiasing = supported && enabled;

            return supported;
         }

         return true;
      }

      wxImage Image;

      std::unique_ptr<wxAutoBufferedPaintDC> BufferedDC;
      std::unique_ptr<wxGraphicsContext> GC;

      Pen CurrentPen { Pen::NoPen };
      Brush CurrentBrush;

      std::shared_ptr<wxGraphicsContextPainterFont> CurrentFont;
      Color CurrentTextColor { Colors::Black };

      Transform CurrentTransform;
      Rect CurrentClipRect { NoClippingRect };

      bool FontDirty { true };
      bool CurrentAntialiasing { true };
   };

   wxGraphicsRenderer* mRenderer;
   wxWindow* mWindow;

   std::vector<std::unique_ptr<Surface>> mPaintTargetsStack;
};

WXGraphicsContextPainter::WXGraphicsContextPainter(
   wxGraphicsRenderer* renderer, wxWindow* window, const wxFont& defaultFont)
    : mPaintTargetStack(std::make_unique<PaintTargetStack>(renderer, window))
    , mDefaultFont(std::make_shared<wxGraphicsContextPainterFont>(*this, defaultFont))
{

}

WXGraphicsContextPainter::WXGraphicsContextPainter(
   wxGraphicsRenderer* renderer, const wxFont& defaultFont)
    : mPaintTargetStack(std::make_unique<PaintTargetStack>(renderer, nullptr))
    , mDefaultFont(std::make_shared<wxGraphicsContextPainterFont>(
         *this, defaultFont))
{
}

WXGraphicsContextPainter::~WXGraphicsContextPainter()
{
}

Size WXGraphicsContextPainter::GetSize() const
{
   return mPaintTargetStack->GetSize();
}

RendererID WXGraphicsContextPainter::GetRendererID() const
{
   return rendererId;
}

std::shared_ptr<PainterFont> WXGraphicsContextPainter::CreateFont(const FontInfo& fontInfo)
{
   return CreateFontFromWX(wxFontFromFontInfo(fontInfo));
}

std::shared_ptr<PainterFont>
WXGraphicsContextPainter::CreateFontFromWX(const wxFont& font)
{
   return std::make_shared<wxGraphicsContextPainterFont>(
      *this, font);
}

std::shared_ptr<PainterImage> WXGraphicsContextPainter::CreateImage(
   PainterImageFormat format, uint32_t width, uint32_t height, const void* data, const void* alphaData)
{
   wxImage image;

   if (data != nullptr)
   {
      switch (format)
      {
      case PainterImageFormat::RGB888:
         image.Create(
            width, height, static_cast<unsigned char*>(const_cast<void*>(data)),
            true);
         break;
      case PainterImageFormat::RGBA8888:
      {
         if (alphaData != nullptr)
         {
            image.Create(
               width, height,
               static_cast<unsigned char*>(const_cast<void*>(data)),
               static_cast<unsigned char*>(const_cast<void*>(alphaData)), true);
         }
         else
         {

            image.Create(width, height);
            image.SetAlpha();

            const unsigned char* u8Data =
               static_cast<const unsigned char*>(data);

            unsigned char* rgbPtr = image.GetData();
            unsigned char* alphaPtr = image.GetAlpha();

            for (uint32_t i = 0; i < width * height; ++i)
            {
               *rgbPtr++ = *u8Data++;
               *rgbPtr++ = *u8Data++;
               *rgbPtr++ = *u8Data++;
               *alphaPtr++ = *u8Data++;
            }
         }
      }
         break;
      default:
         assert(false);
         break;
      }
   }
   else
   {
      image.Create(width, height);

      if (format == PainterImageFormat::RGBA8888)
      {
         image.SetAlpha();
         std::memset(image.GetAlpha(), 0, width * height);
      }
   }

   return std::make_shared<wxGraphicsContextPainterImage>(
      *this, mPaintTargetStack->GetRenderer(), image);
}

std::shared_ptr<PainterImage> WXGraphicsContextPainter::GetSubImage(
   const std::shared_ptr<PainterImage>& image, uint32_t x, uint32_t y,
   uint32_t width, uint32_t height)
{
   if (GetRendererID() != image->GetRendererID())
      return {};

   return std::make_shared<wxGraphicsContextPainterImage>(
      *this, static_cast<const wxGraphicsContextPainterImage&>(*image), x, y,
      width, height);
}

std::shared_ptr<PainterImage> WXGraphicsContextPainter::CreateDeviceImage(
   PainterImageFormat format, uint32_t width, uint32_t height)
{
   return CreateImage(format, width, height);
}

// wxGraphicsContext does not allow clearing the context.
// wxGraphicsContext::Clear() has an empty implementation.
// This is probably OK with the software based renderers,
// but hardware uses Clear to optimize the rendering often.
void WXGraphicsContextPainter::DoClear(const Rect& rect, Color color)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   auto context = mPaintTargetStack->GetCurrentContext();

   auto compostionMode = context->GetCompositionMode();

   // Use wxCOMPOSITION_SOURCE so we overwrite the alpha data of the surface
   context->SetCompositionMode(wxCOMPOSITION_SOURCE);

   UpdateBrush(color);

   context->DrawRectangle(
         rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);

   UpdateBrush(GetCurrentBrush());

   context->SetCompositionMode(compostionMode);
}

void WXGraphicsContextPainter::BeginPaint()
{
   // Nesting the on-screen paint events is not allowed
   assert(!mPaintTargetStack->InPaintEnvent());

   if (mPaintTargetStack->InPaintEnvent())
      return;

   mPaintTargetStack->BeginPaint();

   UpdateBrush(GetCurrentBrush());
   UpdatePen(GetCurrentPen());
   UpdateFont(GetCurrentFont());
   UpdateAntiAliasingState(GetAntiAliasingEnabled());
   UpdateTransform(GetCurrentTransform());
   UpdateClipRect(GetCurrentClipRect());
}

void WXGraphicsContextPainter::EndPaint()
{
   FlushCachedPath();
   mPaintTargetStack->EndPaint();
}

void WXGraphicsContextPainter::UpdateBrush(const Brush& brush)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   mPaintTargetStack->SetBrush(brush);
}

void WXGraphicsContextPainter::UpdatePen(const Pen& pen)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   if (!mPaintTargetStack->IsSamePen(pen))
   {
      FlushCachedPath();
      mPaintTargetStack->SetPen(pen);
   }
}

void WXGraphicsContextPainter::UpdateTransform(const Transform& transform)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   mPaintTargetStack->SetTransform(transform);
}

void WXGraphicsContextPainter::UpdateClipRect(const Rect& rect)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   mPaintTargetStack->SetClipRect(rect);
}

bool WXGraphicsContextPainter::UpdateAntiAliasingState(bool enabled)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return false;

   return mPaintTargetStack->SetAntialiasing(enabled);
}

void WXGraphicsContextPainter::DoDrawPolygon(const Point* pts, size_t count)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   FlushCachedPath();

   mPoints.reserve(count + 1);

   for (size_t i = 0; i < count; ++i)
      mPoints.emplace_back(wxPoint2DDouble(pts[i].x, pts[i].y));

   mPoints.emplace_back(wxPoint2DDouble(pts[0].x, pts[0].y));

  mPaintTargetStack->GetCurrentContext()->DrawLines(mPoints.size(), mPoints.data());

  mPoints.clear();
}

void WXGraphicsContextPainter::DoDrawLines(const Point* pts, size_t count)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   assert(count % 2 == 0);

   auto& path = GetCachedPath();

   for (size_t i = 0; i < count / 2; ++i)
   {
      path.MoveToPoint(pts[2 * i].x, pts[2 * i].y);
      path.AddLineToPoint(pts[2 * i + 1].x, pts[2 * i + 1].y);
   }
}

void WXGraphicsContextPainter::DoDrawRect(const Rect& rect)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   FlushCachedPath();

   mPaintTargetStack->GetCurrentContext()->DrawRectangle(
      rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

void WXGraphicsContextPainter::DoDrawEllipse(const Rect& rect)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   FlushCachedPath();

   mPaintTargetStack->GetCurrentContext()->DrawEllipse(
      rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

void WXGraphicsContextPainter::DoDrawText(
   Point origin, Brush backgroundBrush,
   const std::string_view& text)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   FlushCachedPath();

   mPaintTargetStack->UpdateFont();
   
   wxGraphicsContext* currentContext = mPaintTargetStack->GetCurrentContext();

   const wxGraphicsBrush bgBrush =
      backgroundBrush.GetStyle() != BrushStyle::None ?
         currentContext->CreateBrush(wxBrushFromBrush(backgroundBrush)) :
         wxNullGraphicsBrush;

   currentContext->DrawText(
      wxString::FromUTF8(text.data(), text.size()), origin.x, origin.y, bgBrush);
}

void WXGraphicsContextPainter::DoDrawRotatedText(
   Point origin, float angle, Brush backgroundBrush,
   const std::string_view& text)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   FlushCachedPath();

   mPaintTargetStack->UpdateFont();

   wxGraphicsContext* currentContext = mPaintTargetStack->GetCurrentContext();

   const wxGraphicsBrush bgBrush =
      backgroundBrush.GetStyle() != BrushStyle::None ?
         currentContext->CreateBrush(wxBrushFromBrush(backgroundBrush)) :
         wxNullGraphicsBrush;

   currentContext->DrawText(
      wxString::FromUTF8(text.data(), text.size()), origin.x, origin.y, angle,
      bgBrush);
}

void WXGraphicsContextPainter::DoDrawImage(
   const PainterImage& painterImage, const Rect& rect, const Rect& imageRect)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   if (painterImage.GetRendererID() != GetRendererID())
      return;

   if (rect.size.IsZero() || imageRect.size.IsZero())
      return;

   FlushCachedPath();

   wxGraphicsContext* currentContext = mPaintTargetStack->GetCurrentContext();

   const auto& image =
      static_cast<const wxGraphicsContextPainterImage&>(painterImage);

   auto gcImage = image.Bitmap;

   if (
      !imageRect.origin.IsZero() || uint32_t(imageRect.size.width) != image.Width ||
      uint32_t(imageRect.size.height) != image.Height)
   {
      gcImage = currentContext->CreateSubBitmap(
         gcImage, imageRect.origin.x, imageRect.origin.y, imageRect.size.width,
         imageRect.size.height);
   }
      
   currentContext->DrawBitmap(
      gcImage, rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
}

std::shared_ptr<PainterFont> WXGraphicsContextPainter::GetDefaultFont() const
{
   return mDefaultFont;
}

Size WXGraphicsContextPainter::DoGetTextSize(
   const std::string_view& text, bool) const
{   
   if (!mPaintTargetStack->InPaintEnvent())
       return GetCurrentFont()->GetTextSize(text);

   mPaintTargetStack->UpdateFont();
   
   double width, height;

   mPaintTargetStack->GetCurrentContext()->GetTextExtent(
      wxString::FromUTF8(text.data(), text.size()), &width, &height);

   return { static_cast<float>(width), static_cast<float>(height) };
}

void WXGraphicsContextPainter::UpdateFont(std::shared_ptr<PainterFont> font)
{
   if (font->GetRendererID() != GetRendererID())
      return;

   //! UpdateFont will called again every time paint to new target starts
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   mPaintTargetStack->SetFont(
      std::static_pointer_cast<wxGraphicsContextPainterFont>(font));
}

void WXGraphicsContextPainter::DoDrawRoundedRect(const Rect& rect, float radius)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   FlushCachedPath();

   mPaintTargetStack->GetCurrentContext()->DrawRoundedRectangle(
      rect.origin.x, rect.origin.y, rect.size.width, rect.size.height, radius);
}

void WXGraphicsContextPainter::PushPaintTarget(
   const std::shared_ptr<PainterImage>& image)
{
   if (image->GetRendererID() != GetRendererID())
      return;

   if (mPaintTargetStack->InPaintEnvent())
      FlushCachedPath();

   mPaintTargetStack->Push(std::static_pointer_cast<wxGraphicsContextPainterImage>(image));

   UpdatePen(GetCurrentPen());
   UpdateBrush(GetCurrentBrush());
   UpdateFont(GetCurrentFont());
   UpdateAntiAliasingState(GetAntiAliasingEnabled());
}

void WXGraphicsContextPainter::PopPaintTarget(
   const std::shared_ptr<PainterImage>& image)
{
   FlushCachedPath();
   mPaintTargetStack->Pop(
      std::static_pointer_cast<wxGraphicsContextPainterImage>(image));
}

wxGraphicsPath& WXGraphicsContextPainter::GetCachedPath()
{
   if (mCachedPath == nullptr)
   {
      mCachedPath = std::make_unique<wxGraphicsPath>(
         mPaintTargetStack->GetCurrentContext()->CreatePath());
   }

   return *mCachedPath;
}

void WXGraphicsContextPainter::FlushCachedPath()
{
   if (mCachedPath == nullptr)
      return;

   mPaintTargetStack->GetCurrentContext()->StrokePath(*mCachedPath);
   mCachedPath.reset();
}

std::shared_ptr<PainterPath> WXGraphicsContextPainter::CreatePath()
{
   return std::make_shared<wxGraphicsContextPainterPath>(
      *this, mPaintTargetStack->GetRenderer());
}

void WXGraphicsContextPainter::DrawPath(const PainterPath& path)
{
   if (!mPaintTargetStack->InPaintEnvent())
      return;

   if (path.GetRendererID() != GetRendererID())
      return;

   FlushCachedPath();

   static_cast<const wxGraphicsContextPainterPath&>(path).DrawPath(
      *mPaintTargetStack->GetCurrentContext());
}

RendererID WXGraphicsContextPainterRendererID()
{
   return rendererId;
}

} // namespace graphics::wx
