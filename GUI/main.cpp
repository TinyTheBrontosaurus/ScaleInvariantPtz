// wxhello.cpp
// Version using dynamic event routing
// Robert Roebling, Martin Bernreuther

#include <wx/wx.h>
#include "gui.h"
#include "Video.h"

class MyApp : public wxApp
{
	virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit()
{
	SITrackerFrame *frame = new SITrackerFrame((SITrackerFrame*)0,-1,"Hello World", wxPoint(50,50),
                wxSize(450,350));

	/*frame->Connect( ID_Quit, wxEVT_COMMAND_MENU_SELECTED,
		(wxObjectEventFunction) &MyFrame::OnQuit );
	frame->Connect( ID_About, wxEVT_COMMAND_MENU_SELECTED,
		(wxObjectEventFunction) &MyFrame::OnAbout );
*/
	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}
