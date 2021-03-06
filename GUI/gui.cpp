// -*- C++ -*- generated by wxGlade 0.3.5.1 on Wed Mar 30 19:41:24 2005

#include "gui.h"

enum {
  ID_MENU_ABOUT = wxID_HIGHEST + 1,   //Must be first entry in menu
  ID_MENU_CAL_ZOOM,
  ID_MENU_RUN,
  ID_MENU_APPLY,
  ID_MENU_EXIT, //This should be the last entry in the menu
  
  ID_COMBOBOX_VIDEO_SRC,  
  ID_COMBOBOX_VIDEO_DST
};


BEGIN_EVENT_TABLE(SITrackerFrame, wxFrame)
   EVT_MENU_RANGE        (ID_MENU_ABOUT, ID_MENU_EXIT+1, SITrackerFrame::OnMenu)
   //EVT_MENU              (ID_MENU_EXIT, SITrackerFrame::OnMenu )
   EVT_TEXT              (ID_COMBOBOX_VIDEO_SRC, SITrackerFrame::OnComboboxVideoSrc)
   EVT_TEXT              (ID_COMBOBOX_VIDEO_DST, SITrackerFrame::OnComboboxVideoDst)
END_EVENT_TABLE()

SITrackerFrame::SITrackerFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: SITrackerFrame::SITrackerFrame
    notebook_1 = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, 0);
    notebook_1_pane_5 = new wxPanel(notebook_1, -1);
    notebook_1_pane_4 = new wxPanel(notebook_1, -1);
    notebook_1_pane_3 = new wxPanel(notebook_1, -1);
    notebook_1_pane_2 = new wxPanel(notebook_1, -1);
    notebook_1_pane_1 = new wxPanel(notebook_1, -1);
    frame_1_menubar = new wxMenuBar();
    SetMenuBar(frame_1_menubar);
    wxMenu* menuFile = new wxMenu();
    menuFile->Append(ID_MENU_RUN, wxT("Run"), wxT(""), wxITEM_CHECK);
    menuFile->Append(ID_MENU_APPLY, wxT("Apply"), wxT(""), wxITEM_NORMAL);
    menuFile->Append(ID_MENU_CAL_ZOOM, wxT("Calibrate zoom..."), wxT(""), wxITEM_NORMAL);
    menuFile->Append(ID_MENU_EXIT, wxT("Exit"), wxT(""), wxITEM_NORMAL);
    frame_1_menubar->Append(menuFile, wxT("File"));
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(ID_MENU_ABOUT, wxT("About"), wxT(""), wxITEM_NORMAL);
    frame_1_menubar->Append(menuHelp, wxT("Help"));
    label_1 = new wxStaticText(notebook_1_pane_1, -1, wxT("Output to:"));
    const wxString comboBoxOutputTo_choices[] = {
        wxT("Screen"),
        wxT("File")
    };
    comboBoxOutputTo = new wxComboBox(notebook_1_pane_1, ID_COMBOBOX_VIDEO_DST, wxT(""), wxDefaultPosition, wxDefaultSize, 2, comboBoxOutputTo_choices, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
    label_2 = new wxStaticText(notebook_1_pane_1, -1, wxT("Input from:"));
    const wxString comboBoxInputFrom_choices[] = {
        wxT("Frame Grabber"),
        wxT("File")
    };
    comboBoxInputFrom = new wxComboBox(notebook_1_pane_1, ID_COMBOBOX_VIDEO_SRC, wxT(""), wxDefaultPosition, wxDefaultSize, 2, comboBoxInputFrom_choices, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);
    checkbox_7 = new wxCheckBox(notebook_1_pane_1, -1, wxT("Loop"));
    label_3 = new wxStaticText(notebook_1_pane_1, -1, wxT("Video to output:"));
    const wxString combo_box_3_choices[] = {
        wxT("")
    };
    combo_box_3 = new wxComboBox(notebook_1_pane_1, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 1, combo_box_3_choices, wxCB_DROPDOWN);
    checkbox_1 = new wxCheckBox(notebook_1_pane_1, -1, wxT("Show tracking boxes"));
    checkbox_2 = new wxCheckBox(notebook_1_pane_1, -1, wxT("Show fps"));
    const wxString combo_box_4_choices[] = {
        wxT("")
    };
    combo_box_4 = new wxComboBox(notebook_1_pane_1, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 1, combo_box_4_choices, wxCB_DROPDOWN);
    checkbox_5 = new wxCheckBox(notebook_1_pane_2, -1, wxT("Track"));
    checkbox_6 = new wxCheckBox(notebook_1_pane_2, -1, wxT("Fixate"));
    checkbox_6_copy = new wxCheckBox(notebook_1_pane_2, -1, wxT("Scale Invariant"));
    checkbox_6_copy_1 = new wxCheckBox(notebook_1_pane_2, -1, wxT("Optical Zoom"));
    checkbox_6_copy_2 = new wxCheckBox(notebook_1_pane_2, -1, wxT("Digital Zoom"));
    checkbox_6_copy_3 = new wxCheckBox(notebook_1_pane_2, -1, wxT("Splice zoom out"));
    label_8 = new wxStaticText(notebook_1_pane_2, -1, wxT("Tracker"));
    const wxString combo_box_6_choices[] = {
        wxT("")
    };
    combo_box_6 = new wxComboBox(notebook_1_pane_2, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 1, combo_box_6_choices, wxCB_DROPDOWN);
    button_5 = new wxButton(notebook_1_pane_2, -1, wxT("Options"));
    static_line_1 = new wxStaticLine(notebook_1_pane_2, -1);
    checkbox_4_copy = new wxCheckBox(notebook_1_pane_2, -1, wxT("Measure final size"));
    label_8_copy_copy = new wxStaticText(notebook_1_pane_2, -1, wxT("Tracker"));
    const wxString combo_box_6_copy_copy_choices[] = {
        wxT("")
    };
    combo_box_6_copy_copy = new wxComboBox(notebook_1_pane_2, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 1, combo_box_6_copy_copy_choices, wxCB_DROPDOWN);
    button_5_copy_copy = new wxButton(notebook_1_pane_2, -1, wxT("Options"));
    label_8_copy_1_copy = new wxStaticText(notebook_1_pane_2, -1, wxT("Save as..."));
    text_ctrl_5_copy = new wxTextCtrl(notebook_1_pane_2, -1, wxT(""));
    button_5_copy_1_copy = new wxButton(notebook_1_pane_2, -1, wxT("Browse"));
    label_6 = new wxStaticText(notebook_1_pane_3, -1, wxT("Position 1 (deg)"));
    text_ctrl_1 = new wxTextCtrl(notebook_1_pane_3, -1, wxT("0.0"));
    button_3 = new wxButton(notebook_1_pane_3, -1, wxT("Step"));
    label_7 = new wxStaticText(notebook_1_pane_3, -1, wxT("Position 2 (deg)"));
    text_ctrl_1_copy = new wxTextCtrl(notebook_1_pane_3, -1, wxT("0.0"));
    button_4 = new wxButton(notebook_1_pane_3, -1, wxT("Pulse"));
    label_9 = new wxStaticText(notebook_1_pane_3, -1, wxT("Iterations"));
    text_ctrl_1_copy_1 = new wxTextCtrl(notebook_1_pane_3, -1, wxT("1"));
    static_line_3 = new wxStaticLine(notebook_1_pane_3, -1);
    label_4 = new wxStaticText(notebook_1_pane_3, -1, wxT("Current Position (deg)"));
    button_1 = new wxButton(notebook_1_pane_3, -1, wxT("Update"));
    label_5 = new wxStaticText(notebook_1_pane_3, -1, wxT("0.0"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    button_2 = new wxToggleButton(notebook_1_pane_3, -1, wxT("Realtime"));
    label_6_copy = new wxStaticText(notebook_1_pane_4, -1, wxT("Position 1 (deg)"));
    text_ctrl_1_copy_2 = new wxTextCtrl(notebook_1_pane_4, -1, wxT("0.0"));
    button_3_copy = new wxButton(notebook_1_pane_4, -1, wxT("Step"));
    label_7_copy = new wxStaticText(notebook_1_pane_4, -1, wxT("Position 2 (deg)"));
    text_ctrl_1_copy_copy = new wxTextCtrl(notebook_1_pane_4, -1, wxT("0.0"));
    button_4_copy = new wxButton(notebook_1_pane_4, -1, wxT("Pulse"));
    label_9_copy = new wxStaticText(notebook_1_pane_4, -1, wxT("Iterations"));
    text_ctrl_1_copy_1_copy = new wxTextCtrl(notebook_1_pane_4, -1, wxT("1"));
    static_line_3_copy = new wxStaticLine(notebook_1_pane_4, -1);
    label_4_copy = new wxStaticText(notebook_1_pane_4, -1, wxT("Current Position (deg)"));
    button_1_copy = new wxButton(notebook_1_pane_4, -1, wxT("Update"));
    label_5_copy = new wxStaticText(notebook_1_pane_4, -1, wxT("0.0"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    button_2_copy = new wxToggleButton(notebook_1_pane_4, -1, wxT("Realtime"));
    label_6_copy_copy = new wxStaticText(notebook_1_pane_5, -1, wxT("Magnification 1"));
    text_ctrl_1_copy_2_copy = new wxTextCtrl(notebook_1_pane_5, -1, wxT("1.0"));
    button_3_copy_copy = new wxButton(notebook_1_pane_5, -1, wxT("Step"));
    label_7_copy_copy = new wxStaticText(notebook_1_pane_5, -1, wxT("Magnification 2"));
    text_ctrl_1_copy_copy_copy = new wxTextCtrl(notebook_1_pane_5, -1, wxT("1.0"));
    button_4_copy_copy = new wxButton(notebook_1_pane_5, -1, wxT("Pulse"));
    label_9_copy_copy = new wxStaticText(notebook_1_pane_5, -1, wxT("Iterations"));
    text_ctrl_1_copy_1_copy_copy = new wxTextCtrl(notebook_1_pane_5, -1, wxT("1"));
    checkbox_3 = new wxCheckBox(notebook_1_pane_5, -1, wxT("Optical"));
    checkbox_4 = new wxCheckBox(notebook_1_pane_5, -1, wxT("Digital"));
    static_line_3_copy_copy = new wxStaticLine(notebook_1_pane_5, -1);
    label_4_copy_copy = new wxStaticText(notebook_1_pane_5, -1, wxT("Current Magnification\n         (optical)"));
    button_1_copy_copy = new wxButton(notebook_1_pane_5, -1, wxT("Update"));
    label_5_copy_copy = new wxStaticText(notebook_1_pane_5, -1, wxT("1.0"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    button_2_copy_copy = new wxToggleButton(notebook_1_pane_5, -1, wxT("Realtime"));

    set_properties();
    do_layout();
    // end wxGlade
    
    controller = new Controller();
    controller->start(0);
}


void SITrackerFrame::set_properties()
{
    // begin wxGlade: SITrackerFrame::set_properties
    SetTitle(wxT("Scale Invariance GUI"));
    SetSize(wxSize(273, 328));
    comboBoxOutputTo->SetSelection(-1);
    comboBoxInputFrom->SetSelection(-1);
    combo_box_3->SetSelection(-1);
    combo_box_4->SetSelection(-1);
    combo_box_6->SetSelection(-1);
    combo_box_6_copy_copy->SetSelection(-1);
    text_ctrl_1->SetSize(wxSize(40, 20));
    text_ctrl_1_copy->SetSize(wxSize(40, 20));
    text_ctrl_1_copy_1->SetSize(wxSize(40, 20));
    text_ctrl_1_copy_2->SetSize(wxSize(40, 20));
    text_ctrl_1_copy_copy->SetSize(wxSize(40, 20));
    text_ctrl_1_copy_1_copy->SetSize(wxSize(40, 20));
    text_ctrl_1_copy_2_copy->SetSize(wxSize(40, 20));
    text_ctrl_1_copy_copy_copy->SetSize(wxSize(40, 20));
    text_ctrl_1_copy_1_copy_copy->SetSize(wxSize(40, 20));
    // end wxGlade
}


void SITrackerFrame::do_layout()
{
    // begin wxGlade: SITrackerFrame::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_3_copy_copy = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_4_copy_copy = new wxGridSizer(2, 2, 0, 0);
    wxGridSizer* grid_sizer_6_copy_copy = new wxGridSizer(3, 3, 0, 0);
    wxBoxSizer* sizer_8 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_3_copy = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_4_copy = new wxGridSizer(2, 2, 0, 0);
    wxGridSizer* grid_sizer_6_copy = new wxGridSizer(3, 3, 0, 0);
    wxBoxSizer* sizer_3 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_4 = new wxGridSizer(2, 2, 0, 0);
    wxGridSizer* grid_sizer_6 = new wxGridSizer(3, 3, 0, 0);
    wxBoxSizer* sizer_5 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_3_copy = new wxGridSizer(2, 2, 0, 0);
    wxBoxSizer* sizer_11_copy_1_copy = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_11_copy_copy = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_10 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_11 = new wxBoxSizer(wxVERTICAL);
    wxGridSizer* grid_sizer_5 = new wxGridSizer(6, 4, 5, 3);
    wxGridSizer* grid_sizer_1 = new wxGridSizer(4, 2, 0, 0);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* sizer_6 = new wxBoxSizer(wxVERTICAL);
    grid_sizer_1->Add(label_1, 0, 0, 0);
    grid_sizer_1->Add(comboBoxOutputTo, 0, 0, 0);
    grid_sizer_1->Add(label_2, 0, 0, 0);
    sizer_6->Add(comboBoxInputFrom, 0, 0, 0);
    sizer_6->Add(checkbox_7, 0, 0, 0);
    grid_sizer_1->Add(sizer_6, 1, wxEXPAND, 0);
    grid_sizer_1->Add(label_3, 0, 0, 0);
    grid_sizer_1->Add(combo_box_3, 0, 0, 0);
    sizer_4->Add(checkbox_1, 0, 0, 0);
    sizer_4->Add(checkbox_2, 0, 0, 0);
    grid_sizer_1->Add(sizer_4, 1, wxEXPAND, 0);
    grid_sizer_1->Add(combo_box_4, 0, 0, 0);
    notebook_1_pane_1->SetAutoLayout(true);
    notebook_1_pane_1->SetSizer(grid_sizer_1);
    grid_sizer_1->Fit(notebook_1_pane_1);
    grid_sizer_1->SetSizeHints(notebook_1_pane_1);
    grid_sizer_5->Add(checkbox_5, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(checkbox_6, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(checkbox_6_copy, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(checkbox_6_copy_1, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(checkbox_6_copy_2, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(20, 20, 0, 0, 0);
    grid_sizer_5->Add(checkbox_6_copy_3, 0, 0, 0);
    sizer_10->Add(grid_sizer_5, 1, wxALL|wxEXPAND, 5);
    sizer_11->Add(label_8, 0, 0, 0);
    sizer_11->Add(combo_box_6, 0, 0, 0);
    sizer_11->Add(button_5, 0, 0, 0);
    sizer_10->Add(sizer_11, 1, 0, 0);
    sizer_5->Add(sizer_10, 1, wxEXPAND, 5);
    sizer_5->Add(static_line_1, 0, wxEXPAND, 0);
    grid_sizer_3_copy->Add(checkbox_4_copy, 0, 0, 0);
    sizer_11_copy_copy->Add(label_8_copy_copy, 0, 0, 0);
    sizer_11_copy_copy->Add(combo_box_6_copy_copy, 0, 0, 0);
    sizer_11_copy_copy->Add(button_5_copy_copy, 0, 0, 0);
    grid_sizer_3_copy->Add(sizer_11_copy_copy, 1, 0, 0);
    grid_sizer_3_copy->Add(20, 20, 0, 0, 0);
    sizer_11_copy_1_copy->Add(label_8_copy_1_copy, 0, 0, 0);
    sizer_11_copy_1_copy->Add(text_ctrl_5_copy, 0, 0, 0);
    sizer_11_copy_1_copy->Add(button_5_copy_1_copy, 0, 0, 0);
    grid_sizer_3_copy->Add(sizer_11_copy_1_copy, 1, 0, 0);
    sizer_5->Add(grid_sizer_3_copy, 1, 0, 0);
    notebook_1_pane_2->SetAutoLayout(true);
    notebook_1_pane_2->SetSizer(sizer_5);
    sizer_5->Fit(notebook_1_pane_2);
    sizer_5->SetSizeHints(notebook_1_pane_2);
    grid_sizer_6->Add(label_6, 0, 0, 0);
    grid_sizer_6->Add(text_ctrl_1, 0, 0, 0);
    grid_sizer_6->Add(button_3, 0, 0, 0);
    grid_sizer_6->Add(label_7, 0, 0, 0);
    grid_sizer_6->Add(text_ctrl_1_copy, 0, 0, 0);
    grid_sizer_6->Add(button_4, 0, 0, 0);
    grid_sizer_6->Add(label_9, 0, 0, 0);
    grid_sizer_6->Add(text_ctrl_1_copy_1, 0, 0, 0);
    grid_sizer_6->Add(20, 20, 0, 0, 0);
    sizer_3->Add(grid_sizer_6, 1, wxEXPAND, 0);
    sizer_3->Add(static_line_3, 0, wxEXPAND, 0);
    grid_sizer_4->Add(label_4, 0, 0, 0);
    grid_sizer_4->Add(button_1, 0, 0, 0);
    grid_sizer_4->Add(label_5, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    grid_sizer_4->Add(button_2, 0, 0, 0);
    sizer_3->Add(grid_sizer_4, 1, wxEXPAND, 0);
    notebook_1_pane_3->SetAutoLayout(true);
    notebook_1_pane_3->SetSizer(sizer_3);
    sizer_3->Fit(notebook_1_pane_3);
    sizer_3->SetSizeHints(notebook_1_pane_3);
    grid_sizer_6_copy->Add(label_6_copy, 0, 0, 0);
    grid_sizer_6_copy->Add(text_ctrl_1_copy_2, 0, 0, 0);
    grid_sizer_6_copy->Add(button_3_copy, 0, 0, 0);
    grid_sizer_6_copy->Add(label_7_copy, 0, 0, 0);
    grid_sizer_6_copy->Add(text_ctrl_1_copy_copy, 0, 0, 0);
    grid_sizer_6_copy->Add(button_4_copy, 0, 0, 0);
    grid_sizer_6_copy->Add(label_9_copy, 0, 0, 0);
    grid_sizer_6_copy->Add(text_ctrl_1_copy_1_copy, 0, 0, 0);
    grid_sizer_6_copy->Add(20, 20, 0, 0, 0);
    sizer_3_copy->Add(grid_sizer_6_copy, 1, wxEXPAND, 0);
    sizer_3_copy->Add(static_line_3_copy, 0, wxEXPAND, 0);
    grid_sizer_4_copy->Add(label_4_copy, 0, 0, 0);
    grid_sizer_4_copy->Add(button_1_copy, 0, 0, 0);
    grid_sizer_4_copy->Add(label_5_copy, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    grid_sizer_4_copy->Add(button_2_copy, 0, 0, 0);
    sizer_3_copy->Add(grid_sizer_4_copy, 1, wxEXPAND, 0);
    notebook_1_pane_4->SetAutoLayout(true);
    notebook_1_pane_4->SetSizer(sizer_3_copy);
    sizer_3_copy->Fit(notebook_1_pane_4);
    sizer_3_copy->SetSizeHints(notebook_1_pane_4);
    grid_sizer_6_copy_copy->Add(label_6_copy_copy, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(text_ctrl_1_copy_2_copy, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(button_3_copy_copy, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(label_7_copy_copy, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(text_ctrl_1_copy_copy_copy, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(button_4_copy_copy, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(label_9_copy_copy, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(text_ctrl_1_copy_1_copy_copy, 0, 0, 0);
    sizer_8->Add(checkbox_3, 0, 0, 0);
    sizer_8->Add(checkbox_4, 0, 0, 0);
    grid_sizer_6_copy_copy->Add(sizer_8, 1, wxEXPAND, 0);
    sizer_3_copy_copy->Add(grid_sizer_6_copy_copy, 1, wxEXPAND, 0);
    sizer_3_copy_copy->Add(static_line_3_copy_copy, 0, wxEXPAND, 0);
    grid_sizer_4_copy_copy->Add(label_4_copy_copy, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    grid_sizer_4_copy_copy->Add(button_1_copy_copy, 0, 0, 0);
    grid_sizer_4_copy_copy->Add(label_5_copy_copy, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    grid_sizer_4_copy_copy->Add(button_2_copy_copy, 0, 0, 0);
    sizer_3_copy_copy->Add(grid_sizer_4_copy_copy, 1, wxEXPAND, 0);
    notebook_1_pane_5->SetAutoLayout(true);
    notebook_1_pane_5->SetSizer(sizer_3_copy_copy);
    sizer_3_copy_copy->Fit(notebook_1_pane_5);
    sizer_3_copy_copy->SetSizeHints(notebook_1_pane_5);
    notebook_1->AddPage(notebook_1_pane_1, wxT("I/O"));
    notebook_1->AddPage(notebook_1_pane_2, wxT("Tracking"));
    notebook_1->AddPage(notebook_1_pane_3, wxT("Pan"));
    notebook_1->AddPage(notebook_1_pane_4, wxT("Tilt"));
    notebook_1->AddPage(notebook_1_pane_5, wxT("Zoom"));
    sizer_2->Add(new wxNotebookSizer(notebook_1), 1, wxEXPAND, 0);
    sizer_1->Add(sizer_2, 1, wxEXPAND, 0);
    SetAutoLayout(true);
    SetSizer(sizer_1);
    Layout();
    // end wxGlade
}

void SITrackerFrame::OnMenu(wxCommandEvent& event)
{
   switch (event.GetId())
   {
      case ID_MENU_EXIT:
      {
         
         // TRUE is to force the frame to close
	 Close(true);
         break;
      }
      case ID_MENU_ABOUT:
      {
         char *title = "Scale Invariant Tracker";
         char *author = "Eric D Nelson (edn2065@rit.edu)";
	 char *version = "0.1";
         char msg[100];
	 sprintf( msg, "%s\nAuthor: %s\nCompilation: %s at %s\nVersion %s\n", 
	    title, author, __DATE__, __TIME__, version);
         wxMessageBox(msg, "About", wxOK | wxICON_INFORMATION, this);   
         break;
      }      
      case ID_MENU_RUN:
        frame_1_menubar->Check( ID_MENU_RUN, controller->toggleApp() );
        break;    
      case ID_MENU_CAL_ZOOM:
        printf( "Zoom calibration not yet implemented.\n");
	break;
      case ID_MENU_APPLY:
        controller->applyOptions();
	break;
   }
}

void SITrackerFrame::OnComboboxVideoSrc(wxCommandEvent& event)
{
  if( strcmp( event.GetString(), "File" ) == 0 )
  {
    controller->options.vidSrc = VID_SRC_FILE;
    strcpy(controller->options.vidSrcPrefix,"/home/edn2065/temp/video/test" ); 
      
    controller->options.vidSrcNumDigits = 5;
    strcpy(controller->options.vidSrcSuffix, "jpg");
    controller->options.vidSrcStart = 0;
    controller->options.vidSrcLoop = false;       
    
  }
  else if( strcmp( event.GetString(), "Frame Grabber" ) == 0 )
  {
    controller->options.vidSrc = VID_SRC_FRAME_GRABBER;
    strcpy(controller->options.vidSrcDirectory, "/dev/v4l/");
    strcpy(controller->options.vidSrcFile, "video0");
  }
  else
  {
    controller->options.vidSrc = VID_SRC_NONE;  
  }
}

void SITrackerFrame::OnComboboxVideoDst(wxCommandEvent& event)
{
  if( strcmp( event.GetString(), "File" ) == 0 )
  {
    controller->options.vidDst = VID_DST_FILE;
    strcpy(controller->options.vidDstDirectory,"/home/edn2065/temp/video/" ); 
    strcpy(controller->options.vidDstPrefix, "test" ); 
    strcpy(controller->options.vidDstSuffix, "jpg" );
  }
  else if( strcmp( event.GetString(), "Screen" ) == 0 )
  {
    controller->options.vidDst = VID_DST_SCREEN;
  }
  else
  {
    controller->options.vidDst = VID_DST_NONE;
  }
}

SITrackerFrame::~SITrackerFrame()
{
  delete controller;
}













