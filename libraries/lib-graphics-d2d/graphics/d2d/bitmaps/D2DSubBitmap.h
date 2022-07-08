/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  D2DSubBitmap.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include "graphics/d2d/D2DBitmap.h"

namespace graphics::d2d
{
//! D2DBitmap implementation that represents a sub-bitmap of another D2DBitmap
class D2DSubBitmap final : public D2DBitmap
{
public:
   D2DSubBitmap(std::shared_ptr<D2DBitmap> parent, const Rect& rect);

   void DrawBitmap(
      D2DRenderTarget& target, const Rect& targetRect,
      const Rect& sourceRect) override;

   std::shared_ptr<D2DRenderTarget>
   GetRenderTarget(D2DRenderTarget& parentRenderTarget) override;

   void DrawFinished(D2DRenderTarget& renderTarget) override;

   uint32_t GetWidth() const override;

   uint32_t GetHeight() const override;

   bool IsValid(Painter& painter) const override;

   bool DoAcquireResource(D2DRenderTarget& target) override;

   void DoReleaseResource(D2DRenderTarget& target) override;

   void CleanupDirect2DResources() override;

   std::vector<uint8_t> GetData() const override;

   bool HasAlpha() const noexcept override;

private:
   std::shared_ptr<D2DBitmap> mParent;
   Rect mRect;
};
} // namespace graphics::d2d
