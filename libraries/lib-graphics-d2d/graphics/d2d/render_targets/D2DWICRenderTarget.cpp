/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  D2DWICRenderTarget.cpp

  Dmitry Vedenko

**********************************************************************/
#include "D2DWICRenderTarget.h"
#include "../bitmaps/D2DWICBitmap.h"

#include "../D2DRenderer.h"

namespace graphics::d2d
{

D2DWICRenderTarget::D2DWICRenderTarget(D2DWICBitmap& bitmap)
    : D2DRenderTarget(bitmap.GetRenderer())
    , D2DRenderTargetResource(bitmap.GetRenderer())
    , mOwner(&bitmap)
{
}

D2DWICRenderTarget::D2DWICRenderTarget(
   D2DRenderer& renderer, Microsoft::WRL::ComPtr<IWICBitmap> bitmap)
    : D2DRenderTarget(renderer)
    , D2DRenderTargetResource(renderer)
    , mWICBitmap(std::move(bitmap))
{
   auto factory = renderer.GetD2DFactory();

   D2D1_RENDER_TARGET_PROPERTIES
   properties {
      D2D1_RENDER_TARGET_TYPE_DEFAULT,
      D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
      0,
      0,
      D2D1_RENDER_TARGET_USAGE_NONE,
      D2D1_FEATURE_LEVEL_DEFAULT
   };

   factory->CreateWicBitmapRenderTarget(
      mWICBitmap.Get(), &properties, mRenderTarget.ReleaseAndGetAddressOf());
}

bool D2DWICRenderTarget::SetParent(D2DRenderTarget& parentRenderTarget)
{
   if (mParentTarget == &parentRenderTarget && mRenderTarget != nullptr)
      return true;

   mParentTarget = &parentRenderTarget;

   return AcquireResource(parentRenderTarget);
}

void D2DWICRenderTarget::HandlePostDrawAction(bool successful)
{
   if (successful && mOwner != nullptr)
      mOwner->DrawFinished(*this);
}

bool D2DWICRenderTarget::DoAcquireResource(D2DRenderTarget& target)
{
   if (mOwner == nullptr)
      return mRenderTarget != nullptr;
   
   auto wicBitmap = mOwner->GetWICBitmap();

   if (wicBitmap == nullptr)
      return false;

   Microsoft::WRL::ComPtr<ID2D1Factory> factory;
   target.GetD2DRenderTarget()->GetFactory(factory.GetAddressOf());
   
   D2D1_RENDER_TARGET_PROPERTIES
   properties { D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(
                   DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                0,
                0,
                D2D1_RENDER_TARGET_USAGE_NONE,
                D2D1_FEATURE_LEVEL_DEFAULT };
   
   auto result = factory->CreateWicBitmapRenderTarget(
      wicBitmap, &properties, mRenderTarget.ReleaseAndGetAddressOf());

   return S_OK == result;
}

void D2DWICRenderTarget::DoReleaseResource(D2DRenderTarget& target)
{
   if (mParentTarget == &target || mOwner == nullptr)
      return;

   mParentTarget = nullptr;
   mRenderTarget.Reset();
}

void D2DWICRenderTarget::CleanupDirect2DResources()
{
}

} // namespace graphics::d2d
