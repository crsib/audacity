/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  ViewSurface.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <memory>

namespace graphics::gl
{
struct GLFunctions;
}

namespace graphics::gl::platforms::macocs
{
//! Helper class that incapsulates the IOSurface.
/*!
 * This class is used to create and manage the IOSurface.
 * While IOSurface API is C-only, a fair amount of Obj-C++ is
 * required to setup the NSView properly.
 *
 * To work side-by-side with wxWidgets, a subview is added to the
 * view. This subview is used to render the IOSurface.
 */
class ViewSurface final
{
public:
   ViewSurface(const GLFunctions& functions, void* view);
   ~ViewSurface();

   uint32_t GetWidth() const noexcept;
   uint32_t GetHeight() const noexcept;
   float GetScaleFactor() const noexcept;

   void BeginRendering();
   void BindFramebuffer();
   void EndRendering();

private:
   class Impl;
   std::unique_ptr<Impl> mImpl;
}; // class ViewSurface

} // namespace graphics::gl::platforms::macocs
