#include "Gridsizer.h"

GridSizer::GridSizer(const wxString& title)
    : wxFrame(NULL, -1, title, wxPoint(-1, -1), wxSize(270, 220))
{
    menubar = new wxMenuBar;
    file = new wxMenu;

    SetMenuBar(menubar);

    sizer = new wxBoxSizer(wxVERTICAL);

    display = new wxTextCtrl(this, -1, wxT(""), wxPoint(-1, -1),
        wxSize(-1, -1), wxTE_RIGHT);

    sizer->Add(display, 0, wxEXPAND | wxTOP | wxBOTTOM, 4);
    gs = new wxGridSizer(4, 4, 3, 3);

   
    gs->Add(new wxButton(this, -1, wxT("7")), 0, wxEXPAND);
    gs->Add(new wxButton(this, -1, wxT("8")), 0, wxEXPAND);
    gs->Add(new wxButton(this, -1, wxT("9")), 0, wxEXPAND);


    sizer->Add(gs, 1, wxEXPAND);
    SetSizer(sizer);
    SetMinSize(wxSize(270, 220));

    Centre();
}