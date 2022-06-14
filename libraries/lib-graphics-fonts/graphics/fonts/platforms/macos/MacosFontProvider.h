/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  MacosFontProvider.h

  Dmitry Vedenko

**********************************************************************/
#pragma once

#include "graphics/fonts/FontProvider.h"

namespace graphics::fonts::platforms::macos
{
//! macOS implementation of FontProvider.
/*!
 * macOS does not allow the get the path to the font file.
 * However, there is a long known hack introduced in Skia
 * that allows to extract required font tables from the CGFont.
 *
 * Audacity uses modified versions of this hack, that tries to
 * improve the performance and memory requirements of this process.
 */
class MacosFontProvider final : public FontProvider
{
public:
 std::unique_ptr<FontStream> GetFontStream(const FontInfo& fontInfo) override;

}; // class MacosFontProvider

} // namespace graphics::fonts::platforms::macos
