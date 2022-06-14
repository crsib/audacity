/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  LinuxFontProvider.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include "graphics/fonts/FontProvider.h"

// "linux" is a preprocessor definition that is defined when compiling for Linux
namespace graphics::fonts::platforms::linux_like
{
//! A FontProvider implemenation based on font-config. It is used on Linux.
class LinuxFontProvider final : public FontProvider
{
public:
   LinuxFontProvider();
   ~LinuxFontProvider();

   std::unique_ptr<FontStream> GetFontStream(const FontInfo& fontInfo) override;

private:
   class Fontconfig;
   std::unique_ptr<Fontconfig> mFontConfig;
}; // class LinuxFontProvider

} // namespace graphics::fonts::platforms::linux_like
