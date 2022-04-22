/*  SPDX-License-Identifier: GPL-2.0-or-later */
/**********************************************************************

  Audacity: A Digital Audio Editor

  Painter.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

#include "Color.h"
#include "Pen.h"
#include "Brush.h"
#include "Transform.h"
#include "RendererID.h"

class Painter;

constexpr Rect NoClippingRect = {
   Point {}, Size { std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity() }
};

enum class PainterHorizontalAlignment
{
   Left,
   Center,
   Right
};

enum class PainterVerticalAlignment
{
   Bottom,
   Center,
   Top
};

class GRAPHICS_API PainterObject /* not final */
{
public:
   virtual ~PainterObject() = default;

   RendererID GetRendererID() const noexcept;
protected:
   explicit PainterObject(Painter& painter);
   explicit PainterObject(const RendererID& rendererId);

private:
   RendererID mRendererID;
};

class GRAPHICS_API PainterFont /* not final */ : public PainterObject
{
public:
   struct Metrics final
   {
      int32_t Ascent { 0 };
      int32_t Descent { 0 };
      int32_t Linegap { 0 };
      int32_t LineHeight { 0 };
   };

   virtual std::string_view GetFace() const = 0;
   virtual float GetFontSize() const = 0;

   virtual Metrics GetFontMetrics() const = 0;

   virtual Size GetTextSize(const std::string_view& text) const = 0;
protected:
   PainterFont(Painter& painter);
};

enum class PainterImageFormat
{
   RGB888,
   RGBA8888,
};

class GRAPHICS_API PainterImage /* not final */ : public PainterObject
{
public:
   virtual uint32_t GetWidth() const = 0;
   virtual uint32_t GetHeight() const = 0;

protected:
   PainterImage(Painter& ptr);
};

class GRAPHICS_API PainterStateMutator final
{
public:
   ~PainterStateMutator();

   void SetBrush(const Brush& brush);
   Brush GetBrush() const;

   void SetPen(const Pen& pen);
   Pen GetPen() const;

   void SetAntiAliasingEnabled(bool enabled);
   bool GetAntiAliasingEnabled() const;

   void SetFont(std::shared_ptr<PainterFont> font);
   std::shared_ptr<PainterFont> GetFont() const;

   Painter& GetPainter() noexcept;
   const Painter& GetPainter() const noexcept;

private:
   explicit PainterStateMutator(Painter& painter);
   Painter& mPainter;

   friend class Painter;
};

class GRAPHICS_API PainterTransformMutator final
{
public:
   ~PainterTransformMutator();

   void Translate(float x, float y);
   void Translate(Point pt);

   void Scale(float scale);
   void Scale(float scx, float scy);
   void Scale(Point scale);

   void SetTransform(const Transform& transform);
   Transform GetTransform() const;

   Painter& GetPainter() noexcept;
   const Painter& GetPainter() const noexcept;

private:
   explicit PainterTransformMutator(Painter& painter);
   Painter& mPainter;

   friend class Painter;
};

class GRAPHICS_API PainterClipStateMutator final
{
public:
   ~PainterClipStateMutator();

   void SetClipRect(const Rect& rect, bool intersect = true);
   void SetClipRect(Point origin, Size size, bool intersect = true);
   void SetClipRect(float x, float y, float w, float h, bool intersect = true);

   Rect GetClipRect() const;
   void ResetClipRect();

   Painter& GetPainter() noexcept;
   const Painter& GetPainter() const noexcept;

private:
   explicit PainterClipStateMutator(Painter& painter);
   Painter& mPainter;

   friend class Painter;
};

class GRAPHICS_API PainterPath final
{
public:
   virtual ~PainterPath();

   void LineTo(float x, float y);
   void LineTo(Point pt);

   void MoveTo(float x, float y);
   void MoveTo(Point pt);

   void AddRect(const Rect& rect);
   void AddRect(Point topLeft, Size size);
   void AddRect(float left, float top, float width, float height);

   Painter& GetPainter() noexcept;
   const Painter& GetPainter() const noexcept;

private:
   explicit PainterPath(Painter& painter);
   Painter& mPainter;

   size_t mPathIndex;

   friend class Painter;
};

class GRAPHICS_API PainterOffscreenHolder final
{
public:
   ~PainterOffscreenHolder();

private:
   PainterOffscreenHolder(PainterImage& surface, Painter& painter);

   Painter& mPainter;
   PainterImage& mSurface;

   PainterStateMutator mStateMutator;
   PainterClipStateMutator mClipStateMutator;
   PainterTransformMutator mTransforfmMutator;

   friend class Painter;
};

class GRAPHICS_API PaintEventHolder final
{
public:
   ~PaintEventHolder();

private:
   PaintEventHolder(Painter& painter);

   Painter& mPainter;

   friend class Painter;
};

class GRAPHICS_API Painter /* not final */
{
public:
   Painter();
   virtual ~Painter() noexcept;

   virtual RendererID GetRendererID() const = 0;

   virtual Size GetSize() const = 0;

   PainterStateMutator GetStateMutator();
   Brush GetCurrentBrush() const;
   Pen GetCurrentPen() const;
   bool GetAntiAliasingEnabled() const;
   std::shared_ptr<PainterFont> GetCurrentFont() const;

   PainterTransformMutator GetTransformMutator();
   Transform GetCurrentTransform() const;

   PainterClipStateMutator GetClipStateMutator();
   Rect GetCurrentClipRect() const;
   bool HasClipping() const noexcept;

   void DrawPolygon(const Point* pts, size_t count);

   void DrawLines(const Point* ptr, size_t count);

   void DrawLine(Point start, Point end);
   void DrawLine(float sx, float sy, float ex, float ey);

   void DrawRect(const Rect& rect);
   void DrawRect(Point topLeft, Size size);
   void DrawRect(float left, float top, float width, float height);

   enum class LinearGradientDirection
   {
      RightToLeft,
      LeftToRight,
      TopToBottom,
      BottomToTop
   };
   
   void DrawLinearGradientRect(const Rect& rect, Color from, Color to, LinearGradientDirection direction);
   void DrawLinearGradientRect(Point topLeft, Size size, Color from, Color to, LinearGradientDirection direction);
   void DrawLinearGradientRect(float left, float top, float width, float height, Color from, Color to, LinearGradientDirection direction);

   void DrawRoundedRect(const Rect& rect, float radius);
   void DrawRoundedRect(Point topLeft, Size size, float radius);
   void DrawRoundedRect(float left, float top, float width, float height, float radius);

   void DrawEllipse(const Rect& rect);
   void DrawEllipse(Point topLeft, Size size);
   void DrawEllipse(float left, float top, float width, float height);

   void DrawCircle(Point center, float radius);
   void DrawCircle(float cx, float cy, float radius);

   virtual std::shared_ptr<PainterFont>
   CreateFont(const std::string_view& faceName, float pixelSize) = 0;
   virtual std::shared_ptr<PainterFont> GetDefaultFont() const = 0;

   void DrawText(
      Point origin, const PainterFont& font, const std::string_view& text);

   void DrawText(
      float x, float y, const PainterFont& font, const std::string_view& text);

   void DrawText(
      Point origin, const std::string_view& text);

   void DrawText(
      float x, float y, const std::string_view& text);

   void DrawText(
      Rect rect, const PainterFont& font, const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   void DrawText(
      Point origin, Size size, const PainterFont& font,
      const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   void DrawText(
      float x, float y, float w, float h, const PainterFont& font,
      const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   void DrawText(
      Rect rect, const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   void DrawText(
      Point origin, Size size, const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   void DrawText(
      float x, float y, float w, float h, const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   void DrawRotatedText(
      Point origin, float angle, const PainterFont& font, const std::string_view& text);

   void DrawRotatedText(
      float x, float y, float angle, const PainterFont& font, const std::string_view& text);

   void DrawRotatedText(Point origin, float angle, const std::string_view& text);
   
   void DrawRotatedText(float x, float y, float angle, const std::string_view& text);

   Size GetTextSize(const PainterFont& font, const std::string_view& text) const;

   Size GetTextSize(const std::string_view& text) const;

   PainterPath CreatePath();

   virtual std::unique_ptr<PainterImage> CreateImage(PainterImageFormat format, uint32_t width, uint32_t height, const void* data = nullptr, const void* alphaData = nullptr) = 0;
   virtual std::unique_ptr<PainterImage> GetSubImage(const PainterImage& image, uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
   virtual std::unique_ptr<PainterImage> CreateDeviceImage(PainterImageFormat format, uint32_t width, uint32_t height) = 0;

   void DrawImage(const PainterImage& image, const Rect& rect);
   void DrawImage(const PainterImage& image, Point topLeft, Size size);
   void DrawImage(const PainterImage& image, float left, float top, float width, float height);
   void DrawImage(const PainterImage& image, Point topLeft);
   void DrawImage(const PainterImage& image, float left, float top);

   void DrawImage(const PainterImage& image, const Rect& destRect, const Rect& sourceRect);
   void DrawImage(const PainterImage& image, Point destTopLeft, Size destSize, Point sourceTopLeft, Size sourceSize);
   void DrawImage(const PainterImage& image, float destLeft, float destTop, float destWidth, float destHeight, float sourceLeft, float sourceTop, float sourceWidth, float sourceHeight);

   void DrawImage(const PainterImage& image, const Rect& destRect, Point sourceTopLeft);
   void DrawImage(const PainterImage& image, Point destTopLeft, Point sourceTopLeft);
   void DrawImage(const PainterImage& image, Point destTopLeft, Size destSize, Point sourceTopLeft);
   void DrawImage(const PainterImage& image, float destLeft, float destTop, float destWidth, float destHeight, float sourceLeft, float sourceTop);

   void Clear(Color color = Colors::Transparent);
   void Clear(float x, float y, float width, float height, Color color = Colors::Transparent);
   void Clear(Point origin, Size size, Color color = Colors::Transparent);
   void Clear(const Rect& rect, Color color = Colors::Transparent);
   
   virtual void Flush() = 0;

   PaintEventHolder Paint();
   PainterOffscreenHolder PaintOn(PainterImage& image);

protected:
   virtual void BeginPaint() = 0;
   virtual void EndPaint() = 0;
   virtual void DoClear(const Rect& rect, Color color) = 0;

   virtual void UpdateBrush(const Brush& brush) = 0;
   virtual void UpdatePen(const Pen& pen) = 0;
   virtual void UpdateTransform(const Transform& transform) = 0;
   virtual void UpdateClipRect(const Rect& rect) = 0;
   virtual bool UpdateAntiAliasingState(bool enabled) = 0;
   virtual void UpdateFont(const std::shared_ptr<PainterFont>& font) = 0;

   virtual size_t BeginPath() = 0;

   virtual void MoveTo(size_t pathIndex, Point pt) = 0;
   virtual void LineTo(size_t pathIndex, Point pt) = 0;
   virtual void AddRect(size_t pathIndex, const Rect& rect) = 0;\

   virtual void EndPath(size_t pathIndex) = 0;

   virtual void DoDrawPolygon(const Point* pts, size_t count) = 0;
   virtual void DoDrawLines(const Point* ptr, size_t count) = 0;
   virtual void DoDrawRect(const Rect& rect) = 0;
   virtual void DoDrawRoundedRect(const Rect& rect, float radius) = 0;
   virtual void DoDrawEllipse(const Rect& rect) = 0;

   virtual void DoDrawImage(const PainterImage& image, const Rect& destRect, const Rect& imageRect) = 0;

   virtual void DoDrawText(
      Point origin, const PainterFont& font, Brush backgroundBrush,
      const std::string_view& text) = 0;

   virtual void DoDrawRotatedText(
      Point origin, float angle, const PainterFont& font, Brush backgroundBrush,
      const std::string_view& text) = 0;

   virtual Size DoGetTextSize(const PainterFont& font, const std::string_view& text) const = 0;

   virtual void PushPaintTarget(PainterImage& image) = 0;
   virtual void PopPaintTarget(PainterImage& image) = 0;

   virtual void DoDrawLinearGradientRect(const Rect& rect, Color from, Color to, LinearGradientDirection direction);

private:
   Rect GetImageRect(const PainterImage& image) const;
   
   struct PainterState final
   {
      Pen Pen { Pen::NoPen };
      Brush Brush;
      std::shared_ptr<PainterFont> Font;
      bool Antialiasing { true };
   };

   std::vector<PainterState> mStateStack;
   std::vector<Transform> mTransformStack;
   std::vector<Rect> mClipStateStack;

   friend class PainterStateMutator;
   friend class PainterTransformMutator;
   friend class PainterClipStateMutator;
   friend class PainterPath;
   friend class PainterOffscreenHolder;
   friend class PaintEventHolder;
};
