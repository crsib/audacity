/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

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

namespace graphics
{

class Painter;
class FontInfo;

//! A rect that represents disabled clipping
constexpr Rect NoClippingRect = {
   Point {}, Size { std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity() }
};

//! Horizontal alignment of an object on painter
enum class PainterHorizontalAlignment
{
   Left,
   Center,
   Right
};

//! Vertical alignment of an object on painter
enum class PainterVerticalAlignment
{
   Bottom,
   Center,
   Top
};

//! Base class for all painter objects
class GRAPHICS_API PainterObject /* not final */
{
public:
   virtual ~PainterObject() = default;

   //! Returns the ID of the renderer this object is compatible with
   RendererID GetRendererID() const noexcept;

protected:
   explicit PainterObject(const Painter& painter);
   explicit PainterObject(const RendererID& rendererId);

private:
   RendererID mRendererID;
};

//! Font object to draw text on a painter
class GRAPHICS_API PainterFont /* not final */
   : public PainterObject
{
public:
   //! Basic metrics of the font
   /*!
    * This metrics are read from the font face at a given font size.
    * Unfortunately, there are no strict definitions for the font metrics,
    * so this value may vary or even lie (this has been observed with Noto Thai fonts for example)
    *
    * Unlike other implementations, we match wxWidgets and all the values are absolute, without having a direction
    */
   struct Metrics final
   {
      //! Vertical distance from the horizontal baseline to the highest ‘character’ coordinate in a font face
      float Ascent { 0 };
      //! Vertical distance from the horizontal baseline to the lowest ‘character’ coordinate in a font face.
      /*!
       * Unlike common conventions, this value is absolute to match the behavior of wxWidgets
       */ 
      float Descent { 0 };
      //! Gap between the lines
      float Linegap { 0 };
      //! Height of the line
      float LineHeight { 0 };
   };

   //! Returns the name of the face
   virtual std::string_view GetFace() const = 0;
   //! Returns the point size of the font
   virtual float GetFontSize() const = 0;
   //! Returns the font metrics
   virtual Metrics GetFontMetrics() const = 0;
   //! Measures the text size
   /*!
    * Rasterizing text is not a trivial task. To improve the quality of the result
    * different font engines are using different approaches. This is especially important
    * when the font size is small.
    *
    * This adds an additional layer of complexity when querying the text sizes as well.
    * Freetype 2 grid fitting algorithm can change the text width significantly, but we
    * have to disable grid fitting for rotated texts. Thus this function allows to measure the
    * text in both scenarios.
    */
   virtual Size GetTextSize(const std::string_view& text, bool gridFitted = true) const = 0;

protected:
   PainterFont(Painter& painter);
   PainterFont(const RendererID& rendererId);
};

//! Format of the painter image
enum class PainterImageFormat
{
   RGB888,
   RGBA8888,
};

//! A wrapper around renderer specific image data
/*!
 * Sharing images between painter with the same RendererID is safe,
 * but with a potential performance/memory penalties  
 */
class GRAPHICS_API PainterImage /* not final */
   : public PainterObject
{
public:
   //! Returns the width of the image
   virtual uint32_t GetWidth() const = 0;
   //! Returns the height of the image
   virtual uint32_t GetHeight() const = 0;
   //! Returns true, if this image represents an actual image
   virtual bool IsValid(Painter& painter) const;
   //! Returns the data of image encoded as 24-bit RGB or 32-bit RGBA,
   //! depending on the image format, if possible.
   /*!
    * This method is slow as it potentially requires to read the data from the
    * GPU only resource.
    *
    * This method should not be used in the contexts where performance is important.
    */
   virtual std::vector<uint8_t> GetData() const = 0;
protected:
   PainterImage(Painter& ptr);
   PainterImage(const RendererID& rendererId);
};

//! RAII object that allows to change the painter state.
class GRAPHICS_API PainterStateMutator final
{
public:
   ~PainterStateMutator();

   //! Sets a new brush
   void SetBrush(const Brush& brush);
   //! Returns the current brush
   Brush GetBrush() const;

   //! Sets a new pen
   void SetPen(const Pen& pen);
   //! Returns the current pen
   Pen GetPen() const;

   //! Tries to change the anti-aliasing state
   void SetAntiAliasingEnabled(bool enabled);
   //! Retrieves the current antialiasing state
   bool GetAntiAliasingEnabled() const;

   //! Sets a new font to be used for drawing text
   void SetFont(std::shared_ptr<PainterFont> font);
   //! Gets the current font
   std::shared_ptr<PainterFont> GetFont() const;

   //! Gets the parent object
   Painter& GetPainter() noexcept;
   //! Gets the parent object
   const Painter& GetPainter() const noexcept;

private:
   explicit PainterStateMutator(Painter& painter);
   Painter& mPainter;

   friend class Painter;
};

//! RAII object that allows to change the transform state of the painter
class GRAPHICS_API PainterTransformMutator final
{
public:
   ~PainterTransformMutator();
   //! Translates the current transformation
   void Translate(float x, float y);
   //! Translates the current transformation
   void Translate(Point pt);

   //! Scales the current transformation
   void Scale(float scale);
   //! Scales the current transformation
   void Scale(float scx, float scy);
   //! Scales the current transformation
   void Scale(Point scale);

   //! Sets a new transformation
   void SetTransform(const Transform& transform);
   //! Gets the current transformation
   Transform GetTransform() const;

   //! Gets the parent object
   Painter& GetPainter() noexcept;
   //! Gets the parent object
   const Painter& GetPainter() const noexcept;

private:
   explicit PainterTransformMutator(Painter& painter);
   Painter& mPainter;

   friend class Painter;
};

//! RAII object that allows to change the clipping state of the painter
class GRAPHICS_API PainterClipStateMutator final
{
public:
   ~PainterClipStateMutator();

   //! Sets a new clip rect on the painter
   void SetClipRect(const Rect& rect, bool intersect = true);
   //! Sets a new clip rect on the painter
   void SetClipRect(Point origin, Size size, bool intersect = true);
   //! Sets a new clip rect on the painter
   void SetClipRect(float x, float y, float w, float h, bool intersect = true);

   //! Gets the current clip rect. Returns NoClipRect if clipping is disabled.
   Rect GetClipRect() const;

   //! Disables clipping on the painter
   void ResetClipRect();

   //! Gets the parent object
   Painter& GetPainter() noexcept;
   //! Gets the parent object
   const Painter& GetPainter() const noexcept;

private:
   explicit PainterClipStateMutator(Painter& painter);
   Painter& mPainter;

   friend class Painter;
};

//! An object, that represent a painter path
/*!
 * Supports only polygons without holes and only fill winding
 */
class GRAPHICS_API PainterPath /* not final */ : public PainterObject
{
public:
   //! Moves the current point to the given point, implicitly starting a new figure and ending the previous one.
   void MoveTo(float x, float y);
   //! Moves the current point to the given point, implicitly starting a new figure and ending the previous one.
   void MoveTo(Point pt);

   //! Adds a straight line from the current point to the given point. Updates the current point.
   void LineTo(float x, float y);
   //! Adds a straight line from the current point to the given point. Updates the current point.
   void LineTo(Point pt);

   //! Adds a rectangle as a new closed figure
   void AddRect(const Rect& rect);
   //! Adds a rectangle as a new closed figure
   void AddRect(Point topLeft, Size size);
   //! Adds a rectangle as a new closed figure
   void AddRect(float left, float top, float width, float height);

   //! Ends the current figure. If closed is true, a line between first and last points is added.
   virtual void EndFigure(bool closed) = 0;

protected:
   explicit PainterPath(const Painter& painter);
   explicit PainterPath(const RendererID& painter);

   virtual void DoLineTo(Point pt) = 0;
   virtual void DoMoveTo(Point pt) = 0;
   virtual void DoAddRect(const Rect& rect) = 0;

private:
   friend class Painter;
};

//! RAII object that delimits an off screen paint event (i. e. drawing to the PainterImage)
class GRAPHICS_API PainterOffscreenHolder final
{
public:
   ~PainterOffscreenHolder();
   //! Forcibly ends the paint event and returns the painter to the previous state
   void Flush();

private:
   PainterOffscreenHolder(
      const std::shared_ptr<PainterImage>& surface, Painter& painter);

   Painter& mPainter;
   std::shared_ptr<PainterImage> mSurface;

   PainterStateMutator mStateMutator;
   PainterClipStateMutator mClipStateMutator;
   PainterTransformMutator mTransforfmMutator;

   friend class Painter;
};

//! RAII object that delimits an on screen paint event.
class GRAPHICS_API PaintEventHolder final
{
public:
   ~PaintEventHolder();

private:
   PaintEventHolder(Painter& painter);

   Painter& mPainter;

   friend class Painter;
};

//! Primary class of the Audacity rendering system. Provides methods to draw paths, images and text both on- and off-screen
/*!
 * Painter is a state machine with the following states available:
 * 
 * 1. Pen: defines how the lines are drawn, including the outlines of the polygons;
 * 2. Brush: defines how the polygon interior is drawn and the text color;
 * 3. Font: defines how the text is drawn and measured;
 * 4. Transformation: defines a transformations that is applied to all the points of the subsequent paint commands;
 * 5. Clipping: defines how the objects are clipped.
 *
 * Painter will always discard the paint commands, that lie outside the clip rect or the bounds of the surface.
 *
 * Drawing can only happen inside the paint event. Only text measurement can happen outside the paint event.
 * Paint events can draw to a window the painter is associated with (on-screen event) and to PainterImage objects (off-screen event).
 * 
 * On-screen paint events can happen only from the OnPaint handlers. It is not possible to nest on-screen events.
 * Off-screen paint event can be performed in any state. The off-screen surface is in undefined state during the off-screen event.
 *
 * Painter uses a coordinate system with the (0, 0) located at the top-left corner and Y axis facing down. Painter always uses
 * device independent pixels. I. e. screen scaling, if any, happens inside the implementation.
 *
 * For convenience all the paint method have the overload sets:
 * - Point arguments have an overload with a tuple (x, y)
 * - Rect arguments have an overloads with tuples (x, y, w, h) and (point, size)
 */
class GRAPHICS_API Painter /* not final */
{
public:
   Painter();
   virtual ~Painter() noexcept;

   //! Returns the ID of the renderer that backs this class
   virtual RendererID GetRendererID() const = 0;

   //! Returns the current size of the painter surface.
   /*!
    * Outside the paint event it will return the size of the window
    * associated with the current painter. If no window is associated, the result is undefined.
    *
    * During the paint event it returns the size of the surface that is painter. 
    */
   virtual Size GetSize() const = 0;

   //! Gets RAII object that allows to change the painter state.
   PainterStateMutator GetStateMutator();
   //! Gets the current brush
   Brush GetCurrentBrush() const;
   //! Gets the current pen
   Pen GetCurrentPen() const;
   //! Get the current anti-aliasing state
   bool GetAntiAliasingEnabled() const;
   //! Get the current font
   std::shared_ptr<PainterFont> GetCurrentFont() const;

   //! Gets RAII object that allows to change transformation state of the painter
   PainterTransformMutator GetTransformMutator();
   //! Returns the current transformation state
   Transform GetCurrentTransform() const;

   //! Gets RAII object that allows to change clipping state of the painter
   PainterClipStateMutator GetClipStateMutator();
   //! Get the current clip rect
   Rect GetCurrentClipRect() const;
   //! Returns true, if clipping is enabled
   bool HasClipping() const noexcept;

   //! Draws a closed polygon
   void DrawPolygon(const Point* pts, size_t count);

   //! Draws a set of lines between pts[2i] and pts[2i+1]. If count is odd, last point will be ignored
   void DrawLines(const Point* pts, size_t count);

   //! Draws a line between to points
   void DrawLine(Point start, Point end);
   //! Draws a line between to points
   void DrawLine(float sx, float sy, float ex, float ey);

   //! Draws a rectangle
   void DrawRect(const Rect& rect);
   //! Draws a rectangle
   void DrawRect(Point topLeft, Size size);
   //! Draws a rectangle
   void DrawRect(float left, float top, float width, float height);

   //! Defines a linear gradient direction used by the DrawLinearGradientRect
   enum class LinearGradientDirection
   {
      //! Gradient is horizontal and goes from right to left
      RightToLeft,
      //! Gradient is horizontal and goes from left to right
      LeftToRight,
      //! Gradient is vertical and goes from top to bottom
      TopToBottom,
      //! Gradient is vertical and goes from bottom to top
      BottomToTop
   };

   //! Draws a rect filled with a linear gradient. The current brush state is ignored.
   void DrawLinearGradientRect(const Rect& rect, Color from, Color to, LinearGradientDirection direction);
   //! Draws a rect filled with a linear gradient. The current brush state is ignored.
   void DrawLinearGradientRect(Point topLeft, Size size, Color from, Color to, LinearGradientDirection direction);
   //! Draws a rect filled with a linear gradient. The current brush state is ignored.
   void DrawLinearGradientRect(float left, float top, float width, float height, Color from, Color to, LinearGradientDirection direction);

   //! Draws a rounded rectangle
   void DrawRoundedRect(const Rect& rect, float radius);
   //! Draws a rounded rectangle
   void DrawRoundedRect(Point topLeft, Size size, float radius);
   //! Draws a rounded rectangle
   void DrawRoundedRect(float left, float top, float width, float height, float radius);

   //! Draws an ellipse
   void DrawEllipse(const Rect& rect);
   //! Draws an ellipse
   void DrawEllipse(Point topLeft, Size size);
   //! Draws an ellipse
   void DrawEllipse(float left, float top, float width, float height);

   //! Draws a circle
   void DrawCircle(Point center, float radius);
   //! Draws a circle
   void DrawCircle(float cx, float cy, float radius);

   //! Creates a new PainterFont that matches fontInfo as close as possible.
   /*!
    * The Painter implementation decides, if the fonts are cached.
    * It is safe to share fonts between the Painters with the same RendererID.
    */ 
   virtual std::shared_ptr<PainterFont> CreateFont(const FontInfo& fontInfo) = 0;
   //! Get a default font of the painter
   virtual std::shared_ptr<PainterFont> GetDefaultFont() const = 0;

   //! Draws the text encoded as an UTF-8 string
   void DrawText(Point origin, const std::string_view& text);
   //! Draws the text encoded as an UTF-8 string
   void DrawText(float x, float y, const std::string_view& text);

   //! Draws the text encoded as an UTF-8 string inside the given rect
   void DrawText(
      Rect rect, const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   //! Draws the text encoded as an UTF-8 string inside the given rect
   void DrawText(
      Point origin, Size size, const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   //! Draws the text encoded as an UTF-8 string inside the given rect
   void DrawText(
      float x, float y, float w, float h, const std::string_view& text,
      PainterHorizontalAlignment horizontalAlignement =
         PainterHorizontalAlignment::Left,
      PainterVerticalAlignment verticalAlignment =
         PainterVerticalAlignment::Top);

   //! Draws rotated text encoded as an UTF-8
   void DrawRotatedText(Point origin, float angle, const std::string_view& text);
   //! Draws rotated text encoded as an UTF-8 string
   void DrawRotatedText(float x, float y, float angle, const std::string_view& text);

   //! Gets the size of the the text encoded as an UTF-8 string
   /*
    * This method does not return a precise size of the string.
    * The height will always match the line height in the font metrics.
    */
   Size GetTextSize(const std::string_view& text, bool gridFitted = true) const;

   //! Creates an empty path
   virtual std::shared_ptr<PainterPath> CreatePath() = 0;
   //! Draws the the path
   virtual void DrawPath(const PainterPath& path) = 0;

   //! Creates painter image
   /*!
    * Creates an image with size (width, height), trying to match the pixel format specified.
    *
    * If data is null, image is left uninitialized.
    * If data is not null, and alphaData is null, and format is RGBA8888 - the data is considered to be interleaved.
    */
   virtual std::shared_ptr<PainterImage> CreateImage(
      PainterImageFormat format, uint32_t width, uint32_t height,
      const void* data = nullptr, const void* alphaData = nullptr) = 0;

   //! Creates painter image, that represents a portion of another image
   virtual std::shared_ptr<PainterImage> GetSubImage(
      const std::shared_ptr<PainterImage>& image, uint32_t x, uint32_t y,
      uint32_t width, uint32_t height) = 0;

   //! Creates painter image best suited for the off-screen painting.
   virtual std::shared_ptr<PainterImage> CreateDeviceImage(
      PainterImageFormat format, uint32_t width, uint32_t height) = 0;

   //! Draws the image into the target area specified, scaling the image if needed
   void DrawImage(const PainterImage& image, const Rect& rect);
   //! Draws the image into the target area specified, scaling the image if needed
   void DrawImage(const PainterImage& image, Point topLeft, Size size);
   //! Draws the image into the target area specified, scaling the image if needed
   void DrawImage(const PainterImage& image, float left, float top, float width, float height);

   //! Draws the image at the origin specified without scaling
   void DrawImage(const PainterImage& image, Point topLeft);
   //! Draws the image at the origin specified without scaling
   void DrawImage(const PainterImage& image, float left, float top);

   //! Draws a portion of image into the target area specified, scaling if needed
   void DrawImage(const PainterImage& image, const Rect& destRect, const Rect& sourceRect);
   //! Draws a portion of image into the target area specified, scaling if needed
   void DrawImage(const PainterImage& image, Point destTopLeft, Size destSize, Point sourceTopLeft, Size sourceSize);
   //! Draws a portion of image into the target area specified, scaling if needed
   void DrawImage(const PainterImage& image, float destLeft, float destTop, float destWidth,
      float destHeight, float sourceLeft, float sourceTop, float sourceWidth,
      float sourceHeight);

   //! Draws a portion of image, starting at the sourceTopLeft, into the target area specified, scaling if needed.
   void DrawImage(const PainterImage& image, const Rect& destRect, Point sourceTopLeft);
   //! Draws a portion of image, starting at the sourceTopLeft, into the target area specified without scaling
   void DrawImage(const PainterImage& image, Point destTopLeft, Point sourceTopLeft);
   //! Draws a portion of image, starting at the sourceTopLeft, into the target area specified, scaling if needed.
   void DrawImage(const PainterImage& image, Point destTopLeft, Size destSize, Point sourceTopLeft);
   //! Draws a portion of image, starting at the sourceTopLeft, into the target area specified, scaling if needed.
   void DrawImage(const PainterImage& image,
      float destLeft, float destTop, float destWidth, float destHeight,
      float sourceLeft, float sourceTop);

   //! Clears current surface of the painter
   void Clear(Color color = Colors::Transparent);
   //! Clears the specified portion of the current surface of the painter
   void Clear(float x, float y, float width, float height, Color color = Colors::Transparent);
   //! Clears the specified portion of the current surface of the painter
   void Clear(Point origin, Size size, Color color = Colors::Transparent);
   //! Clears the specified portion of the current surface of the painter
   void Clear(const Rect& rect, Color color = Colors::Transparent);

   //! Starts the on-screen paint event. Can only be called inside the PaintEvent handler.
   PaintEventHolder Paint();
   //! Starts the off-screen paint event.
   PainterOffscreenHolder PaintOn(const std::shared_ptr<PainterImage>& image);

   //! Checks, if the rect is outside the current drawing area
   bool IsRectClipped(float x, float y, float width, float height) const noexcept;
   //! Checks, if the rect is outside the current drawing area
   bool IsRectClipped(Point point, Size size) const noexcept;
   //! Checks, if the rect is outside the current drawing area
   bool IsRectClipped(const Rect& rect) const noexcept;
   //! Checks, if the rect is outside the current drawing area
   bool IsRectClipped(const AABB& aabb) const noexcept;

protected:
   virtual void BeginPaint() = 0;
   virtual void EndPaint() = 0;
   virtual void DoClear(const Rect& rect, Color color) = 0;

   virtual void UpdateBrush(const Brush& brush) = 0;
   virtual void UpdatePen(const Pen& pen) = 0;
   virtual void UpdateTransform(const Transform& transform) = 0;
   virtual void UpdateClipRect(const Rect& rect) = 0;
   virtual bool UpdateAntiAliasingState(bool enabled) = 0;
   virtual void UpdateFont(std::shared_ptr<PainterFont> font) = 0;

   virtual void DoDrawPolygon(const Point* pts, size_t count) = 0;
   virtual void DoDrawLines(const Point* ptr, size_t count) = 0;
   virtual void DoDrawRect(const Rect& rect) = 0;
   virtual void DoDrawRoundedRect(const Rect& rect, float radius) = 0;
   virtual void DoDrawEllipse(const Rect& rect) = 0;

   virtual void DoDrawImage(const PainterImage& image, const Rect& destRect, const Rect& imageRect) = 0;

   virtual void DoDrawText(Point origin, Brush backgroundBrush, const std::string_view& text) = 0;

   virtual void DoDrawRotatedText(Point origin, float angle, Brush backgroundBrush, const std::string_view& text) = 0;

   virtual Size DoGetTextSize(const std::string_view& text, bool gridFitted) const = 0;

   virtual void PushPaintTarget(const std::shared_ptr<PainterImage>& image) = 0;
   virtual void PopPaintTarget(const std::shared_ptr<PainterImage>& image) = 0;

   virtual void DoDrawLinearGradientRect(
      const Rect& rect, Color from, Color to,
      LinearGradientDirection direction);

private:
   Rect GetImageRect(const PainterImage& image) const;

   struct PainterState final
   {
      Pen pen { Pen::NoPen };
      Brush brush;
      std::shared_ptr<PainterFont> font;
      bool antialiasing { false };
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
} // namespace graphics
