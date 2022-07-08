/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WaveBitmapCache.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cmath>
#include <memory>
#include <numeric>

#include "GraphicsDataCache.h"

#include "graphics/Color.h"
#include "graphics/RendererID.h"
#include "waveform/WavePaintParameters.h"

class wxBitmap;
class wxImage;
class WaveDataCache;
class Envelope;

namespace graphics
{
class PainterImage;
class Painter;
} // namespace graphics

//! An element, that contains a rasterized bitmap matching the WaveDataCacheElement
struct GRAPHICS_API WaveBitmapCacheElement final :
    GraphicsDataCacheElementBase
{
   ~WaveBitmapCacheElement();

   void Dispose() override;

   std::shared_ptr<graphics::PainterImage> Bitmap;
   size_t AvailableColumns { 0 };
};

//! Cache containing rasterized bitmaps representing the waveform
class GRAPHICS_API WaveBitmapCache final :
    public GraphicsDataCache<WaveBitmapCacheElement>
{
public:
   WaveBitmapCache(std::shared_ptr<WaveDataCache> dataCache, double sampleRate);
   ~WaveBitmapCache();

   WaveBitmapCache& SetPaintParameters(const WavePaintParameters& params);
   WaveBitmapCache& SetSelection(const ZoomInfo& zoomInfo, double t0, double t1);
   WaveBitmapCache& SetPainter(graphics::Painter& painter);

private:
   bool InitializeElement(
      const GraphicsDataCacheKey& key, WaveBitmapCacheElement& element) override;

   void CheckCache(const ZoomInfo&, double, double) override;

private:
   struct LookupHelper;

   WavePaintParameters mPaintParamters;

   struct   
   {
      int64_t FirstPixel { -1 };
      int64_t LastPixel { -1 };

      bool IsValid() const noexcept
      {
         return FirstPixel < LastPixel;
      }
   } mSelection;
 
   std::unique_ptr<LookupHelper> mLookupHelper;
   std::vector<uint8_t> mImageBuffer;

   const Envelope* mEnvelope { nullptr };
   size_t mEnvelopeVersion { 0 };

   graphics::Painter* mPainter { nullptr };
   graphics::RendererID mRendererID;
};
