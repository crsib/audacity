/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WGLRenderer.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>

#include "graphics/gl/GLRenderer.h"

namespace graphics::gl::platforms::windows
{
//! Windows implementation of GLRenderer.
/*!
 * This implementation loads OpenGL dynamically, the library is not
 * linked to opengl32.dll.
 *
 * It's quite hard to create OpenGL 3 context on Windows.
 *
 * The process is as follows:
 * 1. Create a dummy window.
 * 2. Create a dummy OpenGL context.
 * 3. Get a pointer to the wglCreateContextAttribsARB
 * 4. Create a real OpenGL context.
 * 5. Destroy the dummy OpenGL context and window.
 *
 * On top of that, we create another dummy window, compatible with the
 * real OpenGL context. This window is used to create offscreen contexts.
 *
 * WindowHandle is an alias to the HWND.
 */
class WGLRenderer final : public GLRenderer
{
public:
   WGLRenderer();
   ~WGLRenderer();

   bool IsAvailable() const noexcept override;

   Context& GetResourceContext() override;

   std::unique_ptr<Context> CreateContext(WindowHandle window) override;
   void ContextDestroyed(Context& ctx);

   virtual void BeginRendering(Context& context) override;
   virtual void EndRendering() override;
private:
   class WGLContext;
   class InvisibleWindow;

   std::unique_ptr<InvisibleWindow> mInvisibleWindow;
   std::unique_ptr<WGLContext> mInvisibleWindowContext;

   WGLContext* mCurrentContext { nullptr };
}; // class WGLRenderer
}// namespace graphics::gl::platforms::windows
