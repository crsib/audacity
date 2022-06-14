/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  D2DWICBitmap.cpp

  Dmitry Vedenko

**********************************************************************/
#include "D2DSubBitmap.h"

#include "graphics/d2d/D2DRenderTarget.h"

#include <algorithm>

namespace graphics::d2d
{

D2DSubBitmap::D2DSubBitmap(std::shared_ptr<D2DBitmap> parent, const Rect& rect)
    : D2DBitmap(parent->GetRenderer())
    , mParent(std::move(parent))
    , mRect(rect)
{
}

void D2DSubBitmap::DrawBitmap(
   D2DRenderTarget& target, const Rect& targetRect, const Rect& sourceRect)
{
   const auto maxAvailableWidth = std::max(
      0.0f, std::min(GetWidth() - sourceRect.origin.x, sourceRect.size.width));

   const auto maxAvailableHeight = std::max(
      0.0f,
      std::min(GetHeight() - sourceRect.origin.y, sourceRect.size.height));

   Rect updatedSourceRect { sourceRect.origin + mRect.origin,
                            Size { maxAvailableWidth, maxAvailableHeight } };

   if (updatedSourceRect.size.IsZero())
      return;

   mParent->DrawBitmap(target, targetRect, updatedSourceRect);
}

std::shared_ptr<D2DRenderTarget>
D2DSubBitmap::GetRenderTarget(D2DRenderTarget& parentRenderTarget)
{
   auto renderTarget = mParent->GetRenderTarget(parentRenderTarget);

   if (renderTarget)
      renderTarget->SetClipRect(mRect);

   return renderTarget;
}

void D2DSubBitmap::DrawFinished(D2DRenderTarget& renderTarget)
{
   mParent->DrawFinished(renderTarget);
}

uint32_t D2DSubBitmap::GetWidth() const
{
   return mRect.size.width;
}

uint32_t D2DSubBitmap::GetHeight() const
{
   return mRect.size.height;
}

bool D2DSubBitmap::IsValid(Painter& painter) const
{
   return mParent->IsValid(painter);
}

bool D2DSubBitmap::DoAcquireResource(D2DRenderTarget& target)
{
   return AcquireResource(target);
}

void D2DSubBitmap::DoReleaseResource(D2DRenderTarget& target)
{
   return mParent->ReleaseResource(target);
}

void D2DSubBitmap::CleanupDirect2DResources()
{
}

std::vector<uint8_t> D2DSubBitmap::GetData() const
{
   auto data = mParent->GetData();

   if (data.empty())
      return {};
   
   const auto width = mParent->GetWidth();
   const auto height = mParent->GetHeight();
   const auto bytesPerPixel = mParent->HasAlpha() ? 4 : 3;

   const auto rect = rect_cast<uint32_t>(mRect);

   for (uint32_t row = 0; row < height; ++row)
   {
      uint8_t* outPtr = row * width * bytesPerPixel + data.data();

      const uint8_t* inPtr = (row + rect.origin.y) * width * bytesPerPixel +
                             rect.origin.x * bytesPerPixel + data.data();

      std::memmove(outPtr, inPtr, rect.size.width * bytesPerPixel);
   }

   return data;
}

bool D2DSubBitmap::HasAlpha() const noexcept
{
   return mParent->HasAlpha();
}

} // namespace graphics::d2d
