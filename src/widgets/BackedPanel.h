//
//  BackedPanel.h
//  Audacity
//
//  Created by Paul Licameli on 5/7/16.
//
//

#ifndef __AUDACITY_BACKED_PANEL__
#define __AUDACITY_BACKED_PANEL__

#include "wxPanelWrapper.h" // to inherit


/// \brief BackedPanel is panel that is expected to be repainted frequently.
class AUDACITY_DLL_API BackedPanel /* not final */ : public wxPanelWrapper {
public:
   BackedPanel(wxWindow * parent, wxWindowID id,
               const wxPoint & pos,
               const wxSize & size,
               long style);

   ~BackedPanel();

   void RequestRefresh();

   void OnSize(wxSizeEvent& event);

protected:
   virtual void HandlePaintEvent(wxPaintEvent& evt) = 0;

private:
   void OnPaint(wxPaintEvent& event);

   bool mWaitRefresh { false };

   DECLARE_EVENT_TABLE()
};


#endif
