/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  FontInfo.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <string>
#include <string_view>

namespace graphics
{

//! Font weight as defined in the OpenType specification
enum class FontWeight
{
   Thin = 100,
   ExtraLight = 200,
   Light = 300,
   SemiLight = 350,
   Normal = 400,
   Medium = 500,
   SemiBold = 600,
   Bold = 700,
   ExtraBold = 800,
   Heavy = 900,
   ExtraBlack = 950,
};

//! Font style
enum class FontStyle
{
   Normal,
   Oblique,
   Italic,
};

//! Font stretch as defined in the OpenType specification
enum class FontStretch
{
   Undefined,
   UltraCondensed,
   ExtraCondensed,
   Condensed,
   SemiCondensed,
   Normal,
   SemiExpanded,
   Expanded,
   ExtraExpanded,
   UltraExpanded,
};

//! Information about the font
class GRAPHICS_API FontInfo final
{
public:
   FontInfo() = default;
   FontInfo(const FontInfo&) = default;
   FontInfo(FontInfo&&) = default;
   FontInfo& operator=(const FontInfo&) = default;
   FontInfo& operator=(FontInfo&&) = default;
   ~FontInfo() = default;

   //! Creates a font name from the given parameters
   FontInfo(std::string_view faceName, float pointSize);

   //! Set the face name of the font
   FontInfo& SetFaceName(std::string_view faceName);
   //! Gets the face name of the font
   std::string_view GetFaceName() const noexcept;

   //! Set the point size of the font
   FontInfo& SetPointSize(float pointSize) noexcept;
   //! Gets the point size of the font
   float GetPointSize() const noexcept;

   //! Set the weight of the font
   FontInfo& SetFontWeight(FontWeight weight) noexcept;
   //! Gets the weight of the font
   FontWeight GetFontWeight() const noexcept;

   //! Set the style of the font
   FontInfo& SetFontStyle(FontStyle style) noexcept;
   //! Gets the style of the font
   FontStyle GetFontStyle() const noexcept;

   //! Set the stretch of the font
   FontInfo& SetFontStretch(FontStretch stretch) noexcept;
   //! Gets the stretch of the font
   FontStretch GetFontStretch() const noexcept;

   //! Set the underlining of the font
   FontInfo& SetUnderlined(bool underlined) noexcept;
   //! Gets the underlining of the font
   bool GetUnderlined() const noexcept;

   //! Set the strikeout of the font
   FontInfo& SetStrikethrough(bool strikethrough) noexcept;
   //! Gets the strikeout of the font
   bool GetStrikethrough() const noexcept;

private:
   std::string mFaceName;
   float mPointSize { 0.0 };
   FontWeight mFontWeight { FontWeight::Normal };
   FontStyle mFontStyle { FontStyle::Normal };
   FontStretch mFontStretch { FontStretch::Normal };

   bool mUnderlined { false };
   bool mStrikethrough { false };
};

GRAPHICS_API bool operator==(const FontInfo& lhs, const FontInfo& rhs);
GRAPHICS_API bool operator!=(const FontInfo& lhs, const FontInfo& rhs);
GRAPHICS_API bool operator<(const FontInfo& lhs, const FontInfo& rhs);

} // namespace graphics
