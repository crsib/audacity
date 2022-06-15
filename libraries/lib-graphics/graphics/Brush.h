/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  Brush.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <memory>
#include <vector>
#include <variant>

#include "Color.h"
#include "Point.h"

namespace graphics
{

//! Style of a brush
enum class BrushStyle
{
   //! Brush is effectively disable
   None,
   //! Brush is a solid color
   Solid,
   //! Brush is a linear gradient
   LinearGradient,
};

//! Gradient stop
struct BrushGradientStop final
{
   //! Position of the stop in the range [0, 1]
   float position { 0.0f };
   //! Color of the stop
   Color color;
};

//! A collection of gradient stops
using BrushGradientStops = std::vector<BrushGradientStop>;

//! Data of the gradient
struct BrushGradientData final
{
   //! Position of the first point in paint surface space
   Point firstPoint;
   //! Position of the second point in paint surface space
   Point secondPoint;
   //! Collection of gradient stops. It is expected that it always has at least two elements.
   BrushGradientStops stops;
};

//! Brush object defines how shapes and text are painted
class GRAPHICS_API Brush final
{
public:
   //! Disabled brush
   static const Brush NoBrush;

   //! Creates a solid black brush
   Brush() = default;
   Brush(const Brush&) = default;
   Brush(Brush&&) = default;
   Brush& operator=(const Brush&) = default;
   Brush& operator=(Brush&&) = default;

   //! Creates a brush with the specified style
   Brush(BrushStyle style);
   //! Creates a brush with the specified color
   Brush(Color color);
   //! Creates a brush with the specified style and color
   Brush(BrushStyle style, Color color);

   //! Creates a linear gradient brush
   Brush(Point start, Point end, Color startColor, Color endColor);
   //! Creates a linear gradient brush
   Brush(
      float startX, float startY, float endX, float endY, Color startColor,
      Color endColor);
   //! Creates a linear gradient brush
   Brush(Point start, Point end, BrushGradientStops stops);
   //! Creates a linear gradient brush
   Brush(
      float startX, float startY, float endX, float endY,
      BrushGradientStops stops);

   ~Brush();

   //! Sets the brush style
   void SetStyle(BrushStyle style) noexcept;
   //! Gets the brush style
   BrushStyle GetStyle() const noexcept;

   //! Sets the brush color
   void SetColor(Color color) noexcept;
   //! Gets the brush color
   Color GetColor() const noexcept;

   //! Gets the brush gradient data
   const BrushGradientData* GetGradientData() const noexcept;

   friend GRAPHICS_API bool
   operator==(const Brush& lhs, const Brush& rhs) noexcept;
   friend GRAPHICS_API bool
   operator!=(const Brush& lhs, const Brush& rhs) noexcept;

private:
   BrushStyle mStyle { BrushStyle::Solid };
   Color mColor { Colors::Black };

   std::shared_ptr<BrushGradientData> mBrushData;
};

} // namespace graphics
