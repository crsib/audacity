/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXPainterUtils.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include "graphics/Painter.h"

#include <wx/brush.h>
#include <wx/pen.h>

class wxFont;

namespace graphics::wx
{
//! Creates Pen from wxPen
GRAPHICS_WX_API Pen PenFromWXPen(const wxPen& pen) noexcept;
//! Creates Brush from wxBrush
GRAPHICS_WX_API Brush BrushFromWXBrush(const wxBrush& brush) noexcept;

//! Creates wxPen from Pen
GRAPHICS_WX_API wxPen wxPenFromPen(const Pen& pen) noexcept;
//! Creates wxBrush from Brush
GRAPHICS_WX_API wxBrush wxBrushFromBrush(const Brush& brush) noexcept;

//! Creates Rect from wxRect
GRAPHICS_WX_API Rect RectFromWXRect(const wxRect& rect) noexcept;

} // namespace graphics::wx
