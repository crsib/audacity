/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  FontFace.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <string_view>
#include <memory>
#include <vector>

#include "graphics/Painter.h"
#include "graphics/FontInfo.h"

namespace graphics::fonts
{
class TextLayout;
class FontProvider;

//! Data for single single glyph
struct FontSymbol final
{
   //! RGBA image
   std::vector<Color> bitmap;

   //! Image width
   uint32_t width;
   //! Image height
   uint32_t height;

   //! Additional horizontal offset to apply when drawing the glyph
   int32_t left;
   //! Additional vertical offset to apply when drawing the glyph
   int32_t top;
};

//! Tuple of the point size and DPI
struct FontSize final
{
   float pointSize;
   uint32_t dpi;
};

//! FreeType 2 font face.
/*!
 * This class represent a single FreeType 2 font face.
 *
 * It provides methods to get glyphs, font metrics and allows shaping the text.
 *
 * For shaping it uses HarfBuzz library to calculate the glyphs, but not the
 * positions of the glyphs. This is because the positions calculated by HarfBuzz
 * are not suitable for grid fitted rendering.
 *
 * To improve text sharpness we use FreeType auto hinting. Hinted text does not
 * handle affine transformations; hinting should be disable when text is scaled or
 * rotated.
 *
 * Text layout and geometry varies significantly between hinted and unhinted rendering.
 * To take this into account, FontFace allows to calculate the text layout for both cases.
 */
class GRAPHICS_FONTS_API FontFace final
{
   FontFace(FontProvider& provider, const FontInfo& info, size_t libraryIndex);
public:
   ~FontFace();

   //! Creates TextLayout for the given text and font size.
   std::shared_ptr<TextLayout> CreateTextLayout(FontSize fontSize, std::string_view text, bool hinted) const;
   //! Get a single glyph. glyphIndex is the index of the glyph in the font, not the code point of the character.
   FontSymbol GetFontSymbol(uint32_t pixelSize, uint32_t glyphIndex, bool hinted) const;

   //! Get the font metrics.
   const PainterFont::Metrics& GetMetrics(FontSize fontSize) const noexcept;

   //! Get the font info. Font size is undefined in the returned structure.
   const FontInfo& GetFontInfo() const noexcept;

   //! Returns true, of the font face is valid.
   bool IsOk() const noexcept;

   //! Returns the face index in the library. This index can be used to simplify the glyph lookup.
   size_t GetLibraryIndex() const noexcept;

   //! Converts FontSize to pixel size.
   static uint32_t GetPixelSize(FontSize size) noexcept;

   //! Returns the default DPI value used for the text rendering
   static uint32_t BaseDPI() noexcept;

private:

   FontInfo mFontInfo;

   size_t mLibraryIndex { std::numeric_limits<size_t>::max() };

   class FreeTypeFace;
   std::unique_ptr<FreeTypeFace> mFreetypeFace;

   friend class FontLibrary;
}; // class FontFace
} // namespace graphics::fonts
