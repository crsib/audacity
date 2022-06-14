/*  SPDX-License-Identifier: GPL-2.0-or-later */
/*!********************************************************************

  Audacity: A Digital Audio Editor

  WXPainterFactory.h

  Dmitry Vedenko

**********************************************************************/
#include "WXPainterFactory.h"

#include <wx/graphics.h>
#include <wx/window.h>
#include <wx/log.h>

#include "graphics/Renderer.h"
#include "graphics/Painter.h"

#include "WXFontUtils.h"
#include "Prefs.h"
#include "CodeConversions.h"

namespace graphics::wx
{

namespace
{
StringSetting PreferredRedererID { L"/Graphics/PreferredRedererID", L"" };

std::unique_ptr<graphics::Renderer> CurrentRenderer;
std::unique_ptr<graphics::Painter> MeasuringPainter;

Renderer& GetRenderer()
{
   if (CurrentRenderer != nullptr)
      return *CurrentRenderer;

   auto prefId = PreferredRedererID.Read();

   if (!prefId.empty() && prefId != L"Auto")
   {
      auto rendererID = FindRendererID(audacity::ToUTF8(prefId));

      if (rendererID.IsValid())
         CurrentRenderer = CreateRenderer(rendererID);
   }

   if (CurrentRenderer == nullptr)
      CurrentRenderer = CreateBestRenderer();

   wxLogInfo(
      "Using renderer: %s", CurrentRenderer->GetRendererID().GetName().data());

   return *CurrentRenderer;
}
} // namespace

std::unique_ptr<Painter> CreatePainter(wxWindow* wnd)
{
   if (wnd == nullptr)
      return {};

   wnd->SetBackgroundStyle(wxBG_STYLE_PAINT);

   auto& renderer = GetRenderer();

   return renderer.CreateWindowPainter(
      renderer.ExpectsNativeHandle() ? WindowHandle(wnd->GetHandle()) :
                                       WindowHandle(wnd),
      FontInfoFromWXFont(wnd->GetFont()));
}

std::unique_ptr<Painter> CreateOffscreenPainter()
{
   return GetRenderer().CreateOffscreenPainter(
      FontInfoFromWXFont(*wxNORMAL_FONT));
}

Painter& GetMeasuringPainter()
{
   if (MeasuringPainter == nullptr)
   {
      MeasuringPainter = GetRenderer().CreateMeasuringPainter(
         FontInfoFromWXFont(*wxNORMAL_FONT));
   }

   return *MeasuringPainter;
}

void ShutdownRenderingSystem()
{
   if (CurrentRenderer != nullptr)
   {
      MeasuringPainter = {};
      CurrentRenderer->Shutdown();
   }
   
   CurrentRenderer = {};
}

} // namespace graphics::wx
