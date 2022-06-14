/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXPainterFactory.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>

class wxWindow;
class wxWindowDC;
class wxMemoryDC;
class wxPrinterDC;
class wxDC;

namespace graphics
{
class Painter;
}

namespace graphics::wx
{
//! Creates a painter for the given wxWindow
GRAPHICS_WX_API std::unique_ptr<Painter> CreatePainter(wxWindow* wnd);

//! Creates a painter for the off-screen rendering
GRAPHICS_WX_API std::unique_ptr<Painter> CreateOffscreenPainter();

//! Get an instance of a measuring painter
GRAPHICS_WX_API Painter& GetMeasuringPainter();

//! Shutdowns the rendering system
GRAPHICS_WX_API void ShutdownRenderingSystem();

} // namespace graphics::wx
