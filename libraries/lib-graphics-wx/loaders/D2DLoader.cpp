#include "graphics/RendererID.h"

namespace graphics::d2d
{
GRAPHICS_D2D_API RendererID GetRendererID();
} // namespace graphics::d2d

namespace graphics::wx::loaders
{
auto d2dRendererId = d2d::GetRendererID();
} // namespace graphics::wx::loaders
