/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  EGLRenderer.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <memory>

#include "graphics/gl/Context.h"
#include "graphics/gl/GLRenderer.h"

namespace graphics::gl::platforms::linux_like
{
class EGLFunctions;
class EGLContextWrapper;

//! EGL implementation of GLRenderer.
/*!
 * This implementation uses EGL to create the OpenGL contexts.
 * It is expected, that EGL is available on all the modern implementations of GNU/Linux
 * and *BSD operating system.
 *
 * Audacity requires EGL 1.4 or higher.
 *
 * WindowHandle is a pointer to the GtkWidget. OpenGL context can only be created
 * after the GtkWidget is realized. This requires the actual OpenGL context to be created lazily.
 */
class EGLRenderer final : public GLRenderer
{
public:
   EGLRenderer();
   ~EGLRenderer();

   bool IsAvailable() const noexcept override;

   Context& GetResourceContext() override;

   std::unique_ptr<Context> CreateContext(WindowHandle window) override;
   void ContextDestroyed(Context& ctx);

   void BeginRendering(Context& context) override;
   void EndRendering() override;

private:
   std::unique_ptr<EGLFunctions> mEGLFunctions;
   std::unique_ptr<EGLContextWrapper> mEGLContext;

   EGLContextWrapper* mCurrentContext { nullptr };
}; // class EGLRenderer


} // namespace graphics::gl::platforms::linux_like
