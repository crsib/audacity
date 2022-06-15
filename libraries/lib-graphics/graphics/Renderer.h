/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  Renderer.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>

#include "RendererID.h"

namespace graphics
{
class Painter;
class FontInfo;

//! Opaque handle to a window.
/*!
 * On Windows handle is HWND;
 * On Linux handle is GtkWidget;
 * On macOS handle is NSView;
 */
using WindowHandle = void*;

//! A factory, that creates painters
class GRAPHICS_API Renderer /* not final */
{
public:  
   virtual ~Renderer() noexcept;

   //! Retrieves the ID of the renderer
   virtual RendererID GetRendererID() const noexcept = 0;

   //! Checks, if Renderer is available on the specific system
   virtual bool IsAvailable() const noexcept = 0;

   //! Creates a new painter to draw on the surface of Window
   virtual std::unique_ptr<Painter> CreateWindowPainter(WindowHandle window, const FontInfo& defaultFont) = 0;
   //! Creates a (potentially) lightweight painter, that can be only used to measure texts
   /*!
    * This overload is needed due to the design of wxWidgets graphics system. 
    */
   virtual std::unique_ptr<Painter> CreateMeasuringPainter(const FontInfo& defaultFont) = 0;
   //! Creates a new painter to draw off-screen. I. e. only Painter::PaintOn can be called on it.
   virtual std::unique_ptr<Painter> CreateOffscreenPainter(const FontInfo& defaultFont) = 0;

   //! Returns true, if CreateWindowPainter expects a platform specific handle
   virtual bool ExpectsNativeHandle() const noexcept = 0;

   //! Free all the resources allocated by the renderer
   virtual void Shutdown() = 0;
}; // class Renderer
} // namespace graphics
