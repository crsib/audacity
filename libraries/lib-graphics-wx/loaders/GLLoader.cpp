#include "graphics/RendererID.h"

namespace graphics::gl
{
GRAPHICS_GL_API RendererID GetRendererID();
} // namespace graphics::gl

namespace graphics::wx::loaders
{
auto glRendererID = gl::GetRendererID();
} // namespace graphics::wx::loaders
