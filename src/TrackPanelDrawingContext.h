/**********************************************************************

 Audacity: A Digital Audio Editor

 TrackPanelDrawingContext.h

 Paul Licameli

 **********************************************************************/

#ifndef __AUDACITY_TRACK_PANEL_DRAWING_CONTEXT__
#define __AUDACITY_TRACK_PANEL_DRAWING_CONTEXT__

#include <memory>
#include <wx/mousestate.h> // member variable

class UIHandle;
using UIHandlePtr = std::shared_ptr<UIHandle>;
class wxDC;

namespace graphics
{
class Painter;
}


struct TrackPanelDrawingContext {
   graphics::Painter& painter;
   UIHandlePtr target;
   wxMouseState lastState;

   void *pUserData;

   // This redundancy fixes an MSVC compiler warning:
   TrackPanelDrawingContext() = delete;
};

#endif
