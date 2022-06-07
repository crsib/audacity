/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXColor.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include "graphics/Color.h"

class wxColour;
class wxPen;
class wxBrush;

//! Convert from wxColor to Color
GRAPHICS_WX_API Color ColorFromWXColor(const wxColour& color) noexcept;
//! Gets the color of the wxPen
GRAPHICS_WX_API Color ColorFromWXPen(const wxPen& pen) noexcept;
//! Gets the color of the wxBrush
GRAPHICS_WX_API Color ColorFromWXBrush(const wxBrush& brush) noexcept;
