/*  SPDX-License-Identifier: GPL-2.0-or-later */
/**********************************************************************

  Audacity: A Digital Audio Editor

  GraphicsDataCache.cpp

  Dmitry Vedenko

**********************************************************************/
#include "GraphicsDataCache.h"

#include <algorithm>
#include <cassert>
#include <tuple>
#include <type_traits>

#include "ZoomInfo.h"

namespace
{

bool IsSameTime(double sampleRate, double t0, double t1) noexcept
{
   const auto diff = std::abs(t0 - t1);

   return diff < (1.0 / sampleRate);
}

bool IsSamePPS(double sampleRate, double lhs, double rhs)
{
   return std::abs(1.0 / lhs - 1.0 / rhs) *
             GraphicsDataCacheBase::CacheElementWidth <
          (1.0 / sampleRate);
}

bool IsSameKey(
   double sampleRate, GraphicsDataCacheKey lhs, GraphicsDataCacheKey rhs)
{
   return lhs.FirstSample == rhs.FirstSample &&
          IsSamePPS(sampleRate, lhs.PixelsPerSecond, rhs.PixelsPerSecond);
}

bool IsKeyLess(
   double sampleRate, GraphicsDataCacheKey lhs, GraphicsDataCacheKey rhs)
{
   if (IsSamePPS(sampleRate, lhs.PixelsPerSecond, rhs.PixelsPerSecond))
      return lhs.FirstSample < rhs.FirstSample;
   else
      return lhs.PixelsPerSecond < rhs.PixelsPerSecond;
}
} // namespace

void GraphicsDataCacheBase::Invalidate()
{
   for (auto& item : mLookup)
      DisposeElement(item.Data);

   mLookup.clear();
}

double GraphicsDataCacheBase::GetSampleRate() const noexcept
{
   return mSampleRate;
}

GraphicsDataCacheBase::GraphicsDataCacheBase(double sampleRate)
    : mSampleRate(sampleRate)
{
}

void GraphicsDataCacheElementBase::Dispose()
{
}

GraphicsDataCacheBase::BaseLookupResult
GraphicsDataCacheBase::PerformBaseLookup(
   const ZoomInfo& zoomInfo, double t0, double t1)
{
   if (bool(t0 > t1) || IsSameTime(mSampleRate, t0, t1))
      return {};

   const double pixelsPerSecond = zoomInfo.GetZoom();

   const int64_t left = zoomInfo.TimeToPosition(t0);
   const int64_t right = zoomInfo.TimeToPosition(t1) + 1;

   const int64_t width = right - left;

   const int64_t cacheLeft = left / CacheElementWidth;
   const int64_t cacheRight = right / CacheElementWidth + 1;
   const int64_t cacheItemsCount = cacheRight - cacheLeft;

   const int64_t cacheLeftColumn = cacheLeft * CacheElementWidth;
   const int64_t cacheRightColumn = cacheRight * CacheElementWidth;

   const double samplesPerPixel = mSampleRate / pixelsPerSecond;

   mMaxWidth = std::max(mMaxWidth, width);

   mNewLookupItems.clear();
   mNewLookupItems.reserve(cacheItemsCount);

   const auto ppsMatchRange = std::equal_range(
      mLookup.begin(), mLookup.end(), pixelsPerSecond,
      [sampleRate = mSampleRate](auto lhs, auto rhs)
      {
         if constexpr (std::is_arithmetic_v<std::decay_t<decltype(lhs)>>)
            return !IsSamePPS(sampleRate, lhs, rhs.Key.PixelsPerSecond) &&
                   lhs < rhs.Key.PixelsPerSecond;
         else
            return !IsSamePPS(sampleRate, lhs.Key.PixelsPerSecond, rhs) &&
                   lhs.Key.PixelsPerSecond < rhs;
   });

   for (int64_t itemIndex = 0; itemIndex < cacheItemsCount; ++itemIndex)
   {
      const int64_t column = cacheLeftColumn + itemIndex * CacheElementWidth;

      const int64_t firstSample =
         static_cast<int64_t>(column * samplesPerPixel);

      const auto it = std::find_if(
         ppsMatchRange.first, ppsMatchRange.second,
         [firstSample](auto element)
         { return element.Key.FirstSample == firstSample; });

      if (it == ppsMatchRange.second)
         mNewLookupItems.push_back({ pixelsPerSecond, firstSample });
   }

   ++mCacheAccessIndex;

   if (!CreateNewItems())
   {
      DisposeNewItems();
      return {};
   }

   mLookupHelper.reserve(mLookup.size() + mNewLookupItems.size());

   std::merge(
      mLookup.begin(), mLookup.end(), mNewLookupItems.begin(),
      mNewLookupItems.end(), std::back_inserter(mLookupHelper),
      [sampleRate = mSampleRate](auto lhs, auto rhs) { return IsKeyLess(sampleRate, lhs.Key, rhs.Key); });

   std::swap(mLookup, mLookupHelper);
   mLookupHelper.clear();

   // Find the very first item satisfying the range
   const GraphicsDataCacheKey firstItemKey {
      pixelsPerSecond, int64_t(cacheLeftColumn * samplesPerPixel)
   };

   auto it = FindKey(firstItemKey);

   assert(it != mLookup.end());

   if (it == mLookup.end())
      return {};

   for (int64_t itemIndex = 0; itemIndex < cacheItemsCount; ++itemIndex)
   {
      auto data = it->Data;

      data->LastCacheAccess = mCacheAccessIndex;
      data->AwaitsEviction = false;

      if (!data->IsComplete && data->LastUpdate != mCacheAccessIndex)
      {
         if (!UpdateElement(it->Key, *data))
            return {};
      }

      ++it;
   }

   PerformCleanup();

   it = FindKey(firstItemKey);
   auto last = it;

   std::advance(last, cacheItemsCount);

   return { it, last,
            static_cast<size_t>(std::max(0LL, left - cacheLeftColumn)),
            static_cast<size_t>(std::max(0LL, cacheRightColumn - right)) };
}

bool GraphicsDataCacheBase::CreateNewItems()
{
   for (auto& item : mNewLookupItems)
   {
      item.Data = CreateElement(item.Key);

      if (item.Data == nullptr)
         return false;

      item.Data->LastUpdate = mCacheAccessIndex;
   }

   return true;
}

void GraphicsDataCacheBase::DisposeNewItems()
{
   std::for_each(
      mNewLookupItems.begin(), mNewLookupItems.end(),
      [](auto elem)
      {
         if (elem.Data != nullptr)
            elem.Data->Dispose();
      });
}

GraphicsDataCacheBase::Lookup::iterator
GraphicsDataCacheBase::FindKey(GraphicsDataCacheKey key)
{
   return std::find_if(
      mLookup.begin(), mLookup.end(),
      [sampleRate = mSampleRate, key](auto lhs)
      { return IsSameKey(sampleRate, lhs.Key, key); });
}

void GraphicsDataCacheBase::PerformCleanup()
{
   const int64_t lookupSize = static_cast<int64_t>(mLookup.size());

   const auto allowedItems =
      RoundUp(mMaxWidth, CacheElementWidth) * mCacheSizeMultiplier;

   const int64_t itemsToEvict = lookupSize - allowedItems;

   if (itemsToEvict <= 0)
      return;

   if (itemsToEvict == 1)
   {
      auto it = std::min_element(
         mLookup.begin(), mLookup.end(),
         [](auto lhs, auto rhs)
         { return lhs.Data->LastCacheAccess < rhs.Data->LastCacheAccess; });

      if (it->Data->LastCacheAccess < mCacheAccessIndex)
      {
         DisposeElement(it->Data);
         mLookup.erase(it);
      }
   }
   else
   {
      PerformFullCleanup(lookupSize, itemsToEvict);
   }
}

void GraphicsDataCacheBase::PerformFullCleanup(
   int64_t currentSize, int64_t itemsToEvict)
{
   mLRUHelper.reserve(currentSize);

   for (size_t i = 0; i < currentSize; ++i)
      mLRUHelper.push_back(i);

   std::make_heap(
      mLRUHelper.begin(), mLRUHelper.end(),
      [this](size_t lhs, size_t rhs)
      {
         return mLookup[lhs].Data->LastCacheAccess >
                mLookup[rhs].Data->LastCacheAccess;
      });

   for (int64_t itemIndex = 0; itemIndex < itemsToEvict; ++itemIndex)
   {
      std::pop_heap(mLRUHelper.begin(), mLRUHelper.end());

      const size_t index = mLRUHelper.back();
      mLRUHelper.pop_back();

      auto data = mLookup[index].Data;

      if (data->LastCacheAccess >= mCacheAccessIndex)
         break;

      DisposeElement(data);
      data->AwaitsEviction = true;
   }

   mLookup.erase(
      std::remove_if(
         mLookup.begin(), mLookup.end(),
         [](auto item) { return item.Data->AwaitsEviction; }),
      mLookup.end());

   mLRUHelper.clear();
}
