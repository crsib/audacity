/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXColor.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include "graphics/Color.h"
#include <wx/colour.h>

class wxPen;
class wxBrush;

namespace graphics::wx
{

//! Convert from wxColor to Color
GRAPHICS_WX_API Color ColorFromWXColor(const wxColour& color) noexcept;
//! Gets the color of the wxPen
GRAPHICS_WX_API Color ColorFromWXPen(const wxPen& pen) noexcept;
//! Gets the color of the wxBrush
GRAPHICS_WX_API Color ColorFromWXBrush(const wxBrush& brush) noexcept;
//! Convert from Color to wxColor
GRAPHICS_WX_API wxColour wxColorFromColor(Color color) noexcept;

} // namespace graphics::wx
