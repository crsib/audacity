/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXGraphicsContextPainter.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>
#include <vector>

#include "graphics/Painter.h"
#include "Observer.h"

class wxWindow;
class wxDC;
class wxGraphicsRenderer;
class wxGraphicsContext;
class wxGraphicsPath;
class wxPoint2DDouble;
class wxFont;

namespace graphics::wx
{

//! Gets the RendererID corresponding to the WXGraphicsContextPainter
GRAPHICS_WX_API RendererID WXGraphicsContextPainterRendererID();

//! WXGraphicsContextPainter is a graphics::Painter implementation that uses wxWidgets wxGraphicsContext
/*!
 * This is a fall-back implementation, provided mainly for Linux compatibility.
 * It is not expected that it will be used in any other cases.
 *
 * wxGraphicsContext is not suitable for the real time rendering due to the design limitations.
 * wxWidgets expects, that context is created for each paint event. It is impossible to reuse it between events.
 *
 * Some of the resources associated with the context can survive the context destruction. However, a significant
 * performance penalty can be observed when a resource is used with a new context in some implementations.
 *
 * Default implementations of the wxGraphicsContext are using software rendering:
 * * GDI+ on Windows;
 * * CoreGraphics on macOS;
 * * Cairo on Linux;
 *
 * Windows has an alternative Direct2D implementation, but it suffers heavily from the inefficient resource management.
 *
 * Known issues:
 * * Windows:
 *    * It is not possible to correctly scale images, and especially one pixel images, due to lack of access
 *      to the required functions with the default, GDI+ implementation
 *    * There are visual issues when drawing text to the transparent render target
 *    * No API provided to control how GDI+ handles pixels and pixel centers, which makes it difficult do draw horizontal and vertical
 *      lines precisely while matching other implementations
 */
class GRAPHICS_WX_API WXGraphicsContextPainter final : public Painter
{
public:
   WXGraphicsContextPainter(
      wxGraphicsRenderer* renderer, wxWindow* window,
      const wxFont& defaultFont);

   WXGraphicsContextPainter(
      wxGraphicsRenderer* renderer, const wxFont& defaultFont);

   ~WXGraphicsContextPainter();

   WXGraphicsContextPainter(const WXGraphicsContextPainter&) = delete;
   WXGraphicsContextPainter(WXGraphicsContextPainter&&) = delete;
   WXGraphicsContextPainter&
   operator=(const WXGraphicsContextPainter&) = delete;
   WXGraphicsContextPainter& operator=(WXGraphicsContextPainter&&) = delete;

   Size GetSize() const override;

   RendererID GetRendererID() const override;

   std::shared_ptr<PainterFont> CreateFont(const FontInfo& fontInfo) override;

   std::shared_ptr<PainterFont> CreateFontFromWX(const wxFont& font);

   std::shared_ptr<PainterImage> CreateImage(
      PainterImageFormat format, uint32_t width, uint32_t height,
      const void* data = nullptr, const void* alphaData = nullptr) override;

   std::shared_ptr<PainterImage> GetSubImage(
      const std::shared_ptr<PainterImage>& image, uint32_t x, uint32_t y,
      uint32_t width, uint32_t height) override;

   std::shared_ptr<PainterImage> CreateDeviceImage(
      PainterImageFormat format, uint32_t width, uint32_t height) override;

private:
   void BeginPaint() override;
   void EndPaint() override;

   void DoClear(const Rect& rect, Color color) override;

   void UpdateBrush(const Brush& brush) override;
   void UpdatePen(const Pen& pen) override;

   void UpdateTransform(const Transform& transform) override;

   void UpdateClipRect(const Rect& rect) override;

   bool UpdateAntiAliasingState(bool enabled) override;

   void DoDrawPolygon(const Point* pts, size_t count) override;
   void DoDrawLines(const Point* ptr, size_t count) override;
   void DoDrawRect(const Rect& rect) override;
   void DoDrawEllipse(const Rect& rect) override;

   void DoDrawText(
      Point origin, Brush backgroundBrush,
      const std::string_view& text) override;

   void DoDrawRotatedText(
      Point origin, float angle, Brush backgroundBrush,
      const std::string_view& text) override;

   void DoDrawImage(
      const PainterImage& image, const Rect& rect,
      const Rect& imageRect) override;

   std::shared_ptr<PainterFont> GetDefaultFont() const override;

   Size
   DoGetTextSize(const std::string_view& text, bool gridFitted) const override;

   void UpdateFont(std::shared_ptr<PainterFont> font) override;

   void DoDrawRoundedRect(const Rect& rect, float radius) override;

   void PushPaintTarget(const std::shared_ptr<PainterImage>& image) override;
   void PopPaintTarget(const std::shared_ptr<PainterImage>& image) override;

   class PaintTargetStack;

   std::unique_ptr<PaintTargetStack> mPaintTargetStack;

   std::vector<wxPoint2DDouble> mPoints;
   std::vector<wxPoint2DDouble> mEndPoints;
   std::shared_ptr<PainterFont> mDefaultFont;

   // wxGraphicsContext seems to use paths to draw lines,
   // at least on Windows.
   // Caching improves the performance dramatically.
   wxGraphicsPath& GetCachedPath();
   void FlushCachedPath();

   std::shared_ptr<PainterPath> CreatePath() override;

   void DrawPath(const PainterPath& path) override;

   std::unique_ptr<wxGraphicsPath> mCachedPath;
};

} // namespace graphics::wx
