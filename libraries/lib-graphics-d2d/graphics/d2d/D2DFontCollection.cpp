/*  SPDX-License-Identifier: GPL-2.0-or-later */
/**********************************************************************

  Audacity: A Digital Audio Editor

  D2DFontCollection.cpp

  Dmitry Vedenko

**********************************************************************/
#include "D2DFontCollection.h"

#include "D2DFont.h"

D2DFontCollection::D2DFontCollection(
   const RendererID& rendererId, IDWriteFactory* factory)
    : mRendererId(rendererId)
    , mFactory(factory)
{
}

std::shared_ptr<D2DFont>
D2DFontCollection::GetFont(const FontInfo& fontInfo, uint32_t dpi)
{
   const auto key = std::make_pair(dpi, fontInfo);
   
   auto it = mFontCollection.find(key);

   if (it != mFontCollection.end())
      return it->second;

   auto font = std::make_shared<D2DFont>(mRendererId, mFactory, dpi, fontInfo);

   if (!font->IsValid())
      return {};

   mFontCollection.emplace(key, font);

   return font;
}
