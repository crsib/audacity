/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  TextLayout.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace graphics::fonts
{
//! Single symbol in the text layout.
struct GRAPHICS_FONTS_API TextLayoutSymbol final
{
   //! Glyph index inside the font face
   uint32_t glyphIndex { 0 };
   //! First index in the original UTF-8 string, that matches this glyph
   uint32_t cluster { 0 };
   //! Horizontal position of the glyph
   int32_t x { 0 };
   //! Vertical position of the glyph
   int32_t y { 0 };
}; // struct TextLayoutSymbol

class GRAPHICS_FONTS_API TextLayout final
{
public:
   TextLayout(
      std::vector<TextLayoutSymbol> symbols, uint32_t width, uint32_t height);

   TextLayout(const TextLayout&) = delete;
   TextLayout(TextLayout&&) = delete;
   TextLayout& operator=(const TextLayout&) = delete;
   TextLayout& operator=(TextLayout&&) = delete;

   const std::vector<TextLayoutSymbol>& GetSymbols() const noexcept;

   //! Gets the width of the string
   uint32_t GetWidth() const noexcept;
   //! Gets the height of the string
   uint32_t GetHeight() const noexcept;

private:
   std::vector<TextLayoutSymbol> mSymbols;

   uint32_t mWidth { 0 };
   uint32_t mHeight { 0 };
}; // class TextLayout
} // namespace graphics::fonts
