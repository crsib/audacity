/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  FontRenderer.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <string_view>

#include "graphics/Point.h"
#include "graphics/Brush.h"

namespace graphics::fonts
{
class Font;
class TextLayout;

//! A base class for font renderers.
/*!
 * lib-graphics-fonts does not provide any implementation of the font rendering.
 *
 * Instead, this interface should be implemented by the users of fonts::Font.
 *
 * FontRenderer implementation is a state-full object. Library will invoke
 * GetDPI and IsHinted to calculate the proper TextLayout before invoking
 * Draw method.
 */
class GRAPHICS_FONTS_API FontRenderer /* not final */
{
public:
   virtual ~FontRenderer() noexcept;

   //! Returns the DPI that will be used for drawing
   virtual uint32_t GetDPI() const noexcept = 0;
   //! Returns true, if hinting will be used for drawing
   virtual bool IsHinted() const noexcept = 0;
   //! Draws the TextLayout
   virtual void Draw(const Font& font, const TextLayout& layout, Color textColor) = 0;
}; // class FontRenderer
} // namespace graphics::fonts
