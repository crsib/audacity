/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  D2DBitmap.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>

#include "graphics/Painter.h"
#include "D2DRenderTargetResource.h"

namespace graphics::d2d
{

class D2DRenderTarget;
class D2DRenderer;

//! Base class for the D2D bitmaps. Implements graphics::PainterImage.
class D2DBitmap /* not final */ :
    public PainterImage,
    public D2DRenderTargetResource
{
public:
   explicit D2DBitmap(D2DRenderer& renderer);

   //! Draws a portion of the bitmap to the given rectangle.
   virtual void DrawBitmap(
      D2DRenderTarget& target, const Rect& targetRect,
      const Rect& sourceRect) = 0;

   //! Creates a render target for the bitmap.
   virtual std::shared_ptr<D2DRenderTarget>
   GetRenderTarget(D2DRenderTarget& parentRenderTarget) = 0;

   //! Notifies the bitmap that drawing to it is complete.
   virtual void DrawFinished(D2DRenderTarget& renderTarget) = 0;

   virtual bool HasAlpha() const noexcept = 0;
};

} // namespace graphics::d2d
