/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  D2DRenderer.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>

#include "Observer.h"
#include "graphics/Renderer.h"
#include "graphics/Painter.h"
#include "graphics/RendererID.h"

struct ID2D1Factory;
struct ID2D1StrokeStyle;
struct ID2D1Bitmap;
struct IWICBitmapSource;
struct ID2D1RenderTarget;

namespace graphics
{
class FontInfo;
} // namespace graphics

namespace graphics::d2d
{
class D2DFontCollection;
class D2DPathGeometry;
class D2DTrackedResource;

struct D2DShutdownMessage : Observer::Message
{
};

//! graphics::Renderer implementation for Direct2D
/*!
 * To ensure the best possible compatibility, Audacity does not
 * link against Direct2D and DirectWrite directly. This class is responsible
 * to check, if the system has the necessary libraries installed and
 * to load them.
 *
 * On top of that, this class is responsible for all interactions with
 * Windows Imaging Component interfaces.
 */
class D2DRenderer final :
    public Renderer,
    public Observer::Publisher<D2DShutdownMessage>
{
public:
   D2DRenderer();
   ~D2DRenderer();

   RendererID GetRendererID() const noexcept override;
   bool IsAvailable() const noexcept override;

   void Shutdown() override;

   std::unique_ptr<Painter> CreateWindowPainter(
      WindowHandle window, const FontInfo& defaultFont) override;

   std::unique_ptr<Painter>
   CreateMeasuringPainter(const FontInfo& defaultFont) override;

   std::unique_ptr<Painter>
   CreateOffscreenPainter(const FontInfo& defaultFont) override;

   std::shared_ptr<PainterImage> CreateImage(
      PainterImageFormat format, uint32_t width, uint32_t height,
      const void* data, const void* alphaData);

   D2DFontCollection* GetFontCollection();

   ID2D1Factory* GetD2DFactory() const;

   ID2D1StrokeStyle* GetStrokeStyle(const Pen& pen);

   std::shared_ptr<D2DPathGeometry> CreatePathGeometry();

   bool ExpectsNativeHandle() const noexcept override;

   std::vector<uint8_t> GetImageData(IWICBitmapSource* source, bool hasAlpha) const;

   std::vector<uint8_t> GetImageData(
      ID2D1RenderTarget* target, ID2D1Bitmap* source, bool hasAlpha) const;

private:
   class D2DRendererImpl;
   std::unique_ptr<D2DRendererImpl> mImpl;
};

} // namespace graphics::d2d
