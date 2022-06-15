/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  CGLRenderer.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <memory>

#include "graphics/gl/Context.h"
#include "graphics/gl/GLRenderer.h"


namespace graphics::gl::platforms::macocs
{
class CGLFunctions;
class CGLContext;
//! CoreGraphics implementation of GLRenderer.
/*!
 * This class uses CoreGraphics to create the OpenGL contexts.
 * macOS allows to attach "special" hardware resorces (IOSurface) as a CoreGraphcs layer to the window.
 *
 * OpenGL can render directly into the IOSurface (unlike Metal). This is used by FireFox and Chrome and seems
 * to give the best perfomance-per-watt possible.
 *
 * WindowHandle is a pointer to the NSView.
 */
class CGLRenderer final : public GLRenderer
{
public:
   CGLRenderer();
   ~CGLRenderer();

   bool IsAvailable() const noexcept override;

   Context& GetResourceContext() override;

   std::unique_ptr<Context> CreateContext(WindowHandle window) override;
   void ContextDestroyed(Context& ctx);

   void BeginRendering(Context& context) override;
   void EndRendering() override;

private:
   std::unique_ptr<CGLFunctions> mCGLFunctions;
   std::unique_ptr<CGLContext> mCGLContext;

   CGLContext* mCurrentContext { nullptr };
}; // class EGLRenderer

} // namespace graphics::gl::platforms::maocs
