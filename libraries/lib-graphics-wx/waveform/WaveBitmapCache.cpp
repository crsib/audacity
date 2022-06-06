/*  SPDX-License-Identifier: GPL-2.0-or-later */
/**********************************************************************

  Audacity: A Digital Audio Editor

  WaveBitmapCache.h

  Dmitry Vedenko

**********************************************************************/
#include "WaveBitmapCache.h"

#include <cassert>

#include <wx/bitmap.h>
#include <wx/image.h>

#include "ZoomInfo.h"

#include "waveform/WaveDataCache.h"
#include "graphics/Painter.h"

#include "Envelope.h"
#include "FrameStatistics.h"

// The worst case scenario is:
// blank -> background -> sample -> rms -> sample -> backgroud -> blank
// So we have 7 stops

constexpr size_t ColorFunctionStops = 7;

struct Triplet final
{
   Triplet() = default;

   explicit Triplet(graphics::Color color)
       : r(color.GetRed())
       , g(color.GetGreen())
       , b(color.GetBlue())
   {
   }

   void SetColor(graphics::Color color)
   {
      r = color.GetRed();
      g = color.GetGreen();
      b = color.GetBlue();
   }

   Triplet(const Triplet&) = default;
   Triplet(Triplet&&) = default;

   uint8_t r { 0 };
   uint8_t g { 0 };
   uint8_t b { 0 };

   uint8_t unused { 0 };
};

struct ColorFunction final
{
   std::array<std::pair<Triplet, uint32_t>, ColorFunctionStops> Stops;

   Triplet GetColor(uint32_t row, Triplet defaultColor) const noexcept
   {
      for (auto stop : Stops)
      {
         if (row < stop.second)
            return stop.first;
      }

      return defaultColor;
   }

   void SetStop(size_t index, graphics::Color color, uint32_t position)
   {
      assert(index < Stops.size());

      Stops[index].first.SetColor(color);
      Stops[index].second = position;
   }
};

struct WaveBitmapCache::LookupHelper final
{
   explicit LookupHelper(std::shared_ptr <WaveDataCache> dataCache)
       : DataCache(std::move(dataCache))
   {
   }

   bool PerformLookup(WaveBitmapCache* cache, GraphicsDataCacheKey key)
   {
      DataCache->UpdateViewportWidth(cache->GetMaxViewportWidth());

      auto result = DataCache->PerformLookup(key);

      if (result == nullptr)
         return false;

      auto sw = FrameStatistics::CreateStopwatch(
         FrameStatistics::SectionID::WaveBitmapCachePreprocess);

      const auto columnsCount = result->AvailableColumns;

      if (cache->mPaintParamters.DBScale)
      {
         auto GetDBValue =
            [dbRange = cache->mPaintParamters.DBRange](float value)
         {
            float sign = (value >= 0 ? 1 : -1);

            if (value != 0.)
            {
               float db = LINEAR_TO_DB(fabs(value));
               value = (db + dbRange) / dbRange;

               if (value < 0.0)
                  value = 0.0;

               value *= sign;
            }

            return value;
         };

         for (size_t column = 0; column < columnsCount; ++column)
         {
            auto oldColumn = result->Data[column];

            DBRemappedColumns[column] = { GetDBValue(oldColumn.min),
                                        GetDBValue(oldColumn.max),
                                        GetDBValue(oldColumn.rms) };
         }
      }

      auto GetRowFromValue =
         [min = cache->mPaintParamters.Min, max = cache->mPaintParamters.Max,
          height = cache->mPaintParamters.Height](float value)
      {
         value = (max - value) / (max - min);
         return static_cast<int>(value * (height - 1) + 0.5);
      };

      const auto height = cache->mPaintParamters.Height;

      auto inputData = cache->mPaintParamters.DBScale ?
                          DBRemappedColumns.data() :
                          result->Data.data();

      auto envelope = cache->mEnvelope;

      if (
         envelope != nullptr && (envelope->GetNumberOfPoints() > 0 ||
                                 envelope->GetDefaultValue() != 1.0))
      {
         envelope->GetValues(
            EnvelopeValues.data(), static_cast<int>(EnvelopeValues.size()),
            key.FirstSample / cache->GetSampleRate(),
            1.0 / key.PixelsPerSecond);

         for (size_t column = 0; column < columnsCount; ++column)
         {
            const auto columnData = inputData[column];
            const float envelopeValue = EnvelopeValues[column];

            EnvRemappedColumns[column] = {
               columnData.min * envelopeValue,
               columnData.max * envelopeValue,
               columnData.rms * envelopeValue
            };
         }

         inputData = EnvRemappedColumns.data();
      }

      const bool hasTopBlankArea = cache->mPaintParamters.Max > 1.0;
      const auto globalMaxRow = GetRowFromValue(cache->mPaintParamters.Max);
      const auto globalMinRow = GetRowFromValue(cache->mPaintParamters.Min) + 1;

      const auto blankColor = cache->mPaintParamters.BlankColor;

      const auto backgroundColors = cache->mPaintParamters.BackgroundColors;
      const auto sampleColors = cache->mPaintParamters.SampleColors;
      const auto rmsColors = cache->mPaintParamters.RMSColors;
      const auto clipColors = cache->mPaintParamters.ClippingColors;

      auto firstPixel = int64_t(key.FirstSample / cache->GetSampleRate() * key.PixelsPerSecond + 0.5);

      const auto selFirst = cache->mSelection.FirstPixel;
      const auto selLast = cache->mSelection.LastPixel;

      const bool showClipping = cache->mPaintParamters.ShowClipping;

      for (size_t column = 0; column < columnsCount; ++column)
      {
         const bool selected = firstPixel >= selFirst && firstPixel < selLast;
         ++firstPixel;

         const auto columnData = inputData[column];
         auto& function = ColorFunctions[column];

         if (showClipping && (columnData.min <= -MAX_AUDIO || columnData.max >= MAX_AUDIO))
         {
            function.SetStop(
               0, selected ? clipColors.Selected : clipColors.Normal, height);

            continue;
         }

         size_t stopIndex = 0;

         if (hasTopBlankArea)
            function.SetStop(stopIndex++, blankColor, globalMaxRow);

         const auto maxRow = GetRowFromValue(columnData.max);

         if (maxRow > 0)
         {
            function.SetStop(
               stopIndex++,
               selected ? backgroundColors.Selected : backgroundColors.Normal,
               maxRow);
         }

         if (maxRow >= height)
            continue;

         const auto positiveRMSRow = GetRowFromValue(columnData.rms);

         if (maxRow < positiveRMSRow)
         {
            function.SetStop(
               stopIndex++,
               selected ? sampleColors.Selected : sampleColors.Normal,
               positiveRMSRow);
         }

         if (positiveRMSRow >= height)
            continue;

         const auto negativeRMSRow = GetRowFromValue(-columnData.rms);

         if (positiveRMSRow < negativeRMSRow)
         {
            function.SetStop(
               stopIndex++, selected ? rmsColors.Selected : rmsColors.Normal,
               negativeRMSRow);
         }

         if (negativeRMSRow >= height)
            continue;

         const auto minRow = GetRowFromValue(columnData.min);

         function.SetStop(
            stopIndex++, selected ? sampleColors.Selected : sampleColors.Normal,
            minRow);

         if (minRow < globalMinRow)
         {
            function.SetStop(
               stopIndex++,
               selected ? backgroundColors.Selected : backgroundColors.Normal,
               globalMinRow);
         }

         if (globalMinRow < height)
            function.SetStop(stopIndex++, blankColor, height);
      }

      AvailableColumns = columnsCount;
      IsComplete = result->IsComplete;

      return true;
   }

   std::shared_ptr<WaveDataCache> DataCache;

   std::array<ColorFunction, GraphicsDataCacheBase::CacheElementWidth>
      ColorFunctions;

   WaveCacheElement::Columns DBRemappedColumns;

   std::array<double, GraphicsDataCacheBase::CacheElementWidth> EnvelopeValues;
   WaveCacheElement::Columns EnvRemappedColumns;

   size_t AvailableColumns { 0 };
   bool IsComplete { 0 };
};

WaveBitmapCache::WaveBitmapCache(
   std::shared_ptr<WaveDataCache> dataCache, double sampleRate)
    : GraphicsDataCache(sampleRate)
    , mLookupHelper(std::make_unique<LookupHelper>(std::move(dataCache)))
{
}

WaveBitmapCache::~WaveBitmapCache()
{
}

WaveBitmapCacheElement::~WaveBitmapCacheElement()
{
}

void WaveBitmapCacheElement::Dispose()
{
   Bitmap = {};
}

WaveBitmapCache&
WaveBitmapCache::SetPaintParameters(const WavePaintParameters& params)
{
   if (mPaintParamters != params)
   {
      mPaintParamters = params;
      mEnvelope = params.AttachedEnvelope;
      mEnvelopeVersion = mEnvelope != nullptr ? mEnvelope->GetVersion() : 0;

      Invalidate();
   }

   return *this;
}

WaveBitmapCache& WaveBitmapCache::SetSelection(
   const ZoomInfo& zoomInfo, double t0, double t1)
{
   const auto empty =
      t0 > t1 || ((t1 - t0) < std::numeric_limits<double>::epsilon());

   const auto first = empty ? int64_t(-1) : zoomInfo.TimeToPosition(t0);
   const auto last =
      empty ? int64_t(-1) : std::max(zoomInfo.TimeToPosition(t1), first + 1);

   const bool needsUpdate =
      mSelection.FirstPixel != first || mSelection.LastPixel != last;

   if (needsUpdate)
   {
      mSelection.FirstPixel = first;
      mSelection.LastPixel = last;

      Invalidate();
   }

   return *this;
}

void WaveBitmapCache::CheckCache(const ZoomInfo&, double, double)
{
   if (mEnvelope != nullptr && mEnvelopeVersion != mEnvelope->GetVersion())
   {
      mEnvelopeVersion = mEnvelope->GetVersion();
      Invalidate();
   }
}

bool WaveBitmapCache::InitializeElement(
   const GraphicsDataCacheKey& key, WaveBitmapCacheElement& element)
{
   if (mPaintParamters.Height == 0)
      return false;

   if (!mLookupHelper->PerformLookup(this, key))
   {
      element.Bitmap = mPainter->CreateImage(
         graphics::PainterImageFormat::RGB888, 1, mPaintParamters.Height,
         nullptr);

      return true;
   }

   auto sw = FrameStatistics::CreateStopwatch(
      FrameStatistics::SectionID::WaveBitmapCache);

   const auto columnsCount = mLookupHelper->AvailableColumns;

   const auto defaultColor = Triplet(mPaintParamters.BlankColor);

   const auto height = static_cast<uint32_t>(mPaintParamters.Height);

   mImageBuffer.reserve(3 * columnsCount * height);
   auto rowData = mImageBuffer.data();

   for (uint32_t row = 0; row < height; ++row)
   {
      auto colorFunction = mLookupHelper->ColorFunctions.data();

      for (size_t pixel = 0; pixel < columnsCount; ++pixel)
      {
         const auto color = colorFunction->GetColor(row, defaultColor);

         *rowData++ = color.r;
         *rowData++ = color.g;
         *rowData++ = color.b;

         ++colorFunction;
      }
   }

   element.AvailableColumns = columnsCount;
   element.IsComplete = mLookupHelper->IsComplete;

   element.Bitmap = mPainter->CreateImage(
      graphics::PainterImageFormat::RGB888, columnsCount, height,
      mImageBuffer.data());

   return true;
}

WaveBitmapCache& WaveBitmapCache::SetPainter(graphics::Painter& painter)
{
   if (mRendererID != painter.GetRendererID())
   {
      mRendererID = painter.GetRendererID();
      Invalidate();
   }

   mPainter = &painter;

   return *this;
}
