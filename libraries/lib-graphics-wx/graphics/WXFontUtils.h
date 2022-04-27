/*  SPDX-License-Identifier: GPL-2.0-or-later */
/**********************************************************************

  Audacity: A Digital Audio Editor

  WXFontUtils.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>
#include <wx/font.h>

#include "graphics/FontInfo.h"

class Painter;
class PainterFont;

GRAPHICS_WX_API std::shared_ptr<PainterFont>
FontFromWXFont(Painter& painter, const wxFont& font);

GRAPHICS_WX_API FontInfo FontInfoFromWXFont(const wxFont& font);
GRAPHICS_WX_API wxFont wxFontFromFontInfo(const FontInfo& fontInfo);
