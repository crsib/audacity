/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  GLRenderer.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>

#include "graphics/Renderer.h"
#include "graphics/RendererID.h"

#include "Observer.h"

namespace graphics
{
class Painter;
class FontInfo;
} // namespace graphics

namespace graphics::gl
{
extern const RendererID OpenGLRendererID;

struct GLFunctions;
class Context;
class ProgramLibrary;
class GLFontRenderer;

struct RendererDestroyedMessage : Observer::Message {};

//! OpenGL implementation of the Renderer interface
/*!
 * WindowHanle type depends on the platform.
 * * On Windows it is HWND
 * * On Linux it is GtkWidget*
 * * On macOS it is NSView*
 */
class GLRenderer /* not final */ :
    public Renderer,
    public Observer::Publisher<RendererDestroyedMessage>
{
public:
   GLRenderer();
   virtual ~GLRenderer();

   RendererID GetRendererID() const noexcept override;

   virtual Context& GetResourceContext() = 0;
   virtual std::unique_ptr<Context> CreateContext(void* window) = 0;

   std::unique_ptr<Painter> CreateWindowPainter(WindowHandle window, const FontInfo& defaultFont) override;
   std::unique_ptr<Painter> CreateMeasuringPainter(const FontInfo& defaultFont) override;
   std::unique_ptr<Painter> CreateOffscreenPainter(const FontInfo& defaultFont) override;

   const ProgramLibrary& GetProgramLibrary() const;
   GLFontRenderer& GetFontRenderer() const;

   virtual void BeginRendering(Context& context) = 0;
   virtual void EndRendering() = 0;

   void Shutdown() override;

   bool ExpectsNativeHandle() const noexcept override;

private:
   std::shared_ptr<ProgramLibrary> mProgramLibrary;
   std::unique_ptr<GLFontRenderer> mFontRenderer;
}; // class GLRenderer

bool RegisterRendererFactory(std::function<std::unique_ptr<GLRenderer>()> factory);
} // namespace graphics::gl
