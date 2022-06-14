/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  TextLayout.cpp

  Dmitry Vedenko

**********************************************************************/
#include "TextLayout.h"

#include <algorithm>

namespace graphics::fonts
{
   
TextLayout::TextLayout(
   std::vector<TextLayoutSymbol> symbols, uint32_t width, uint32_t height)
    : mSymbols(std::move(symbols))
    , mWidth(width)
    , mHeight(height)
{
}

const std::vector<TextLayoutSymbol>& TextLayout::GetSymbols() const noexcept
{
   return mSymbols;
}

uint32_t TextLayout::GetWidth() const noexcept
{
   return mWidth;
}

uint32_t TextLayout::GetHeight() const noexcept
{
   return mHeight;
}

} // namespace graphics::fonts
