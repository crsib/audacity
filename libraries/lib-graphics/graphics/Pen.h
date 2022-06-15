/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  Pen.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>

#include "Color.h"

namespace graphics
{

//! Style of a pen.
enum class PenStyle
{
   //! Pen is disabled
   None,
   //! Pen strokes using a solid line.
   Solid,
   //! Pen strokes using a dotted line.
   Dot,
   //! Pen strokes using long dash pattern.
   LongDash,
   //! Pen strokes using short dash pattern.
   ShortDash,
   //! Pen strokes using dot-dash pattern.
   DotDash
};

//! Pen object defines how lines are drawn.
class GRAPHICS_API Pen final
{
public:
   //! Disabled pen
   static const Pen NoPen;

   //! Creates a solid black pen with width 1.
   Pen() = default;
   Pen(const Pen&) = default;
   Pen(Pen&&) = default;
   Pen& operator=(const Pen&) = default;
   Pen& operator=(Pen&&) = default;

   //! Creates a pen with specified style.
   Pen(PenStyle style);
   //! Creates a pen with specified color.
   Pen(Color color);
   //! Creates a pen with specified color and width.
   Pen(Color color, uint32_t width);
   //! Creates a pen with specified color and style.
   Pen(PenStyle style, Color color);
   //! Creates a pen with specified color, width and style.
   Pen(PenStyle style, Color color, uint32_t width);

   //! Sets the style of the pen.
   void SetStyle(PenStyle style) noexcept;
   //! Gets the style of the pen.
   PenStyle GetStyle() const noexcept;

   //! Sets the color of the pen.
   void SetColor(Color color) noexcept;
   //! Gets the color of the pen.
   Color GetColor() const noexcept;

   //! Sets the width of the pen.
   void SetWidth(uint32_t width) noexcept;
   //! Gets the width of the pen.
   uint32_t GetWidth() const noexcept;

   friend GRAPHICS_API bool operator==(const Pen& lhs, const Pen& rhs) noexcept;
   friend GRAPHICS_API bool operator!=(const Pen& lhs, const Pen& rhs) noexcept;

private:
   PenStyle mStyle { PenStyle::Solid };

   Color mColor { Colors::Black };
   uint32_t mWidth { 1 };
};

} // namespace graphics
