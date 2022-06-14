/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXFontUtils.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>
#include <wx/font.h>

#include "graphics/FontInfo.h"

namespace graphics
{
class Painter;
class PainterFont;
} // namespace graphics

namespace graphics::wx
{

//! Construct PainterFont from the wxFont
GRAPHICS_WX_API std::shared_ptr<PainterFont>
FontFromWXFont(Painter& painter, const wxFont& font);

//! Gets the FontInfo from the wxFont
GRAPHICS_WX_API FontInfo FontInfoFromWXFont(const wxFont& font);
//! Constructs wxFont from the FontInfo
GRAPHICS_WX_API wxFont wxFontFromFontInfo(const FontInfo& fontInfo);

} // namespace graphics::wx
