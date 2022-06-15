/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  GLRenderer.cpp

  Dmitry Vedenko

**********************************************************************/
#include "GLRenderer.h"

#include "Context.h"
#include "GLPainter.h"
#include "ProgramLibrary.h"
#include "GLFontRenderer.h"

#include "graphics/fonts/FontLibrary.h"
#include "graphics/fonts/Font.h"

namespace graphics::gl
{
namespace
{
std::unique_ptr<GLRenderer> renderer;
std::vector<std::function<std::unique_ptr<GLRenderer>()>> RendererFactories;

std::unique_ptr<Renderer> CreateGLRenderer()
{
   for (const auto& factory : RendererFactories)
   {
      auto renderer = factory();

      if (renderer != nullptr && renderer->IsAvailable())
         return renderer;
   }

   return {};
}
}

bool RegisterRendererFactory(std::function<std::unique_ptr<GLRenderer>()> factory)
{
   RendererFactories.emplace_back(std::move(factory));

   return true;
}

const RendererID OpenGLRendererID = RegisterRenderer(
   RendererPriority::Preferred, "OpenGL 3.2", CreateGLRenderer);


GLRenderer::GLRenderer()
    : mProgramLibrary(std::make_shared<ProgramLibrary>(*this))
    , mFontRenderer(std::make_unique<GLFontRenderer>(*this))
{
}

GLRenderer::~GLRenderer()
{
}

RendererID GLRenderer::GetRendererID() const noexcept
{
   return OpenGLRendererID;
}

std::unique_ptr<Painter>
GLRenderer::CreateWindowPainter(WindowHandle window, const FontInfo& defaultFont)
{
   auto context = CreateContext(window);

   if (context == nullptr)
      return {};

   return std::make_unique<GLPainter>(*this, std::move(context), defaultFont);
}

std::unique_ptr<Painter>
GLRenderer::CreateMeasuringPainter(const FontInfo& defaultFont)
{
   return std::make_unique<GLPainter>(*this, GetResourceContext(), defaultFont);
}

std::unique_ptr<Painter>
GLRenderer::CreateOffscreenPainter(const FontInfo& defaultFont)
{
   return std::make_unique<GLPainter>(*this, GetResourceContext(), defaultFont);
}

const ProgramLibrary& GLRenderer::GetProgramLibrary() const
{
   return *mProgramLibrary;
}

GLFontRenderer& GLRenderer::GetFontRenderer() const
{
   return *mFontRenderer;
}

void GLRenderer::Shutdown()
{
   mProgramLibrary = {};

   Publish(RendererDestroyedMessage {});
}

bool GLRenderer::ExpectsNativeHandle() const noexcept
{
   return true;
}

GRAPHICS_GL_API RendererID GetRendererID()
{
   return OpenGLRendererID;
}


} // namespace graphics::gl
