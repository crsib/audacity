/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  D2DPainter.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <vector>

#include "graphics/Painter.h"
#include "graphics/RendererID.h"
#include "graphics/FontInfo.h"

namespace graphics::d2d
{
class D2DRenderer;
class D2DRenderTarget;
class D2DFont;

//! Direct2D implementation of the graphics::Painter interface.
/*!
 * This implementation provides reasonable performance and is expected to work for most,
 * if not all, Audacity users.
 *
 * Direct2D is available starting with Windows Vista SP2 with Platform Update and provides
 * a higher level interface over the Direct3D API.
 *
 * Implementation uses DirectWrite for text shaping and rendering. This provides a good
 * quality result, however the performance is not as good as it would be when using FreeType on top
 * of the Direct3D API.
 *
 * Another major performance draw-back is the path rendering. It targets much more complex cases
 * than Audacity needs at a cost of performance. On top of that, Direct2D does not provide a method
 * to draw a single triangle, so the path rendering is used there as well.
 *
 * This implementation is real-time capable, but it is still slower than OpenGL (or potential Direct3D 11) painter,
 * so it is considered to have the PreferredFallback priority.
 */
class D2DPainter final : public Painter
{
public:
   D2DPainter(
      D2DRenderer& renderer, std::shared_ptr<D2DRenderTarget> target,
      const FontInfo& defaultFont);

   D2DRenderTarget* GetCurrentRenderTarget() const;

private:
   RendererID GetRendererID() const override;

   Size GetSize() const override;

   std::shared_ptr<PainterFont> CreateFont(const FontInfo& fontInfo) override;

   std::shared_ptr<PainterFont> GetDefaultFont() const override;

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

   std::shared_ptr<PainterPath> CreatePath() override;

   void DrawPath(const PainterPath& path) override;

   D2DRenderer& mRenderer;
   FontInfo mDefaultFont;

   std::vector<std::shared_ptr<D2DRenderTarget>> mRenderTargetStack;
   std::shared_ptr<D2DFont> mCurrentD2DFont;
};

} // namespace graphics::d2d
