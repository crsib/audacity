/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  FontProvider.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace graphics
{
class FontInfo;
}

namespace graphics::fonts
{
//! Base class for the font data streams.
class GRAPHICS_FONTS_API FontStream /* not final */
{
public:
   virtual ~FontStream() noexcept;

   //! Perform read on a stream.
   /*!
    * Behavior is defined in FreeType docs:
    * https://freetype.org/freetype2/docs/reference/ft2-system_interface.html#ft_stream_iofunc
    */ 
   virtual unsigned long StreamRead(
      unsigned long offset, unsigned char* buffer, unsigned long count) = 0;
   //! Returns the face index in the font stream. 0 most of the time.
   virtual long GetFaceIndex() const = 0;
};

//! Memory backed FontStream.
class GRAPHICS_FONTS_API MemoryFontStream : public FontStream
{
public:
   explicit MemoryFontStream(std::vector<unsigned char> data);

   unsigned long StreamRead(
      unsigned long offset, unsigned char* buffer,
      unsigned long count) override;

   long GetFaceIndex() const override;

private:
   std::vector<unsigned char> mData;
};

//! Provider, that given the FontInfo return a FontStream,
//! that can be used to read the matching font.
class GRAPHICS_FONTS_API FontProvider /* not final*/
{
public:
   virtual ~FontProvider() noexcept;
 
   virtual std::unique_ptr<FontStream> GetFontStream(const FontInfo& fontInfo) = 0;
}; // class FontProvider
} // namespace graphics::fonts
