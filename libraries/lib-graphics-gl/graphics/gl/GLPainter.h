/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  GLPainter.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include "graphics/Painter.h"

namespace graphics::fonts
{
class Font;
}

namespace graphics::gl
{

class Context;
class GLRenderer;

class PaintTarget;
class PaintTargetsStack;
class StrokeGenerator;

//! OpenGL implementation of the Painter class
/*!
 * OpenGL provides the best performance for drawing.
 * However, OpenGL support really varies from platform to platform.
 *
 * OpenGL is guaranteed to work only on macOS. While it is deprecated, Apple
 * still provides a usable OpenGL implementation even on M1. It is not clear
 * when and if OpenGL will be removed from macOS.
 *
 * On Windows, OpenGL requires the user to install a driver from the manufacturer.
 * Drivers provided by Windows Update may not have OpenGL support.
 *
 * On Linux, Mesa and a set of drivers is needed.
 *
 * Audacity uses OpenGL 3.2. It was announced mid 2009 and most of the hardware should
 * support it now. (Intel has gained support for it since March 2011)
 *
 * Implementation uses lib-graphics-fonts to render text.
 */ 
class GLPainter final : public Painter
{
public:
   GLPainter(GLRenderer& renderer, Context& context, const FontInfo& defaultFont);
   GLPainter(GLRenderer& renderer, std::unique_ptr<Context> context, const FontInfo& defaultFont);

   ~GLPainter();

   RendererID GetRendererID() const override;

   Size GetSize() const override;

   std::shared_ptr<PainterFont> CreateFont(const FontInfo& fontInfo) override;

   std::shared_ptr<PainterFont> GetDefaultFont() const override;

   std::shared_ptr<PainterPath> CreatePath() override;

   void DrawPath(const PainterPath& path) override;

   std::shared_ptr<PainterImage> CreateImage(
      PainterImageFormat format, uint32_t width, uint32_t height,
      const void* data = nullptr, const void* alphaData = nullptr) override;

   std::shared_ptr<PainterImage> GetSubImage(
      const std::shared_ptr<PainterImage>& image, uint32_t x, uint32_t y,
      uint32_t width, uint32_t height) override;

   std::shared_ptr<PainterImage> CreateDeviceImage(
      PainterImageFormat format, uint32_t width, uint32_t height) override;

   void BeginPaint() override;

   void EndPaint() override;

   void DoClear(const Rect& rect, Color color) override;

   void UpdateBrush(const Brush& brush) override;

   void UpdatePen(const Pen& pen) override;

   void UpdateTransform(const Transform& transform) override;

   void UpdateClipRect(const Rect& rect) override;

   bool UpdateAntiAliasingState(bool enabled) override;

   void UpdateFont(std::shared_ptr<PainterFont> font) override;

   void DoDrawPolygon(const Point* pts, size_t count) override;

   void DoDrawLines(const Point* ptr, size_t count) override;

   void DoDrawRect(const Rect& rect) override;

   void DoDrawRoundedRect(const Rect& rect, float radius) override;

   void DoDrawEllipse(const Rect& rect) override;

   void DoDrawImage(
      const PainterImage& image, const Rect& destRect,
      const Rect& imageRect) override;

   void DoDrawText(
      Point origin, Brush backgroundBrush,
      const std::string_view& text) override;

   void DoDrawRotatedText(
      Point origin, float angle, Brush backgroundBrush,
      const std::string_view& text) override;

   Size DoGetTextSize(
      const std::string_view& text,
      bool gridFitted) const override;

   void PushPaintTarget(const std::shared_ptr<PainterImage>& image) override;

   void PopPaintTarget(const std::shared_ptr<PainterImage>& image) override;

   StrokeGenerator& GetStrokeGenerator();

   float GetScale() const noexcept;

private:
   GLRenderer& mRenderer;
   Context& mContext;

   std::unique_ptr<Context> mOwnedContext;

   std::shared_ptr<PainterFont> mDefaultFont;

   std::unique_ptr<PaintTargetsStack> mTargetsStack;
   PaintTarget* mCurrentPaintTarget { nullptr };

   Transform mCurrentTransform;

   Brush mCurrentBrush;
   Pen mCurrentPen;

   std::shared_ptr<fonts::Font> mCurrentFont;

   std::unique_ptr<StrokeGenerator> mStrokeGenerator;

   bool mInPaint { false };
}; // class GLPainter

} // namespace graphics::gl
