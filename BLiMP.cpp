// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
class MyFrame : public wxFrame
{
public:
    MyFrame();
   
private:
    //Menu code start
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    //Menu code stop
    void OpenFileHandeler(wxCommandEvent& event);
    void OnDropFiles(wxDropFilesEvent& event);
};
enum
{
    ID_Hello = 1
};
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Hello World")
{ //Menu code start
    wxMenu* menuFile = new wxMenu;       
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
        "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
   
    //Menu code stop



    //Drag and drop write out
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    wxTextCtrl* dropTarget = new wxTextCtrl(this, wxID_ANY, _("Drop files onto me!"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    
    dropTarget->DragAcceptFiles(true);
    pSizer->Add(dropTarget, 1, wxEXPAND, 0);

    SetSizer(pSizer);
    Layout();
    Centre();

    dropTarget->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MyFrame::OnDropFiles), NULL, this);
    //Btn Testing start

    wxButton* fileBtn = new wxButton(this, wxID_FILE, "OpenFileBtn", wxPoint(2, 8), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
    
    // fileBtn->Create(this, wxID_FILE, "OpenFileBtn", wxPoint(2,8), wxDefaultSize,1L,wxDefaultValidator,"FileOpener");
    fileBtn->SetSize(100, 4);
    pSizer->Add(fileBtn, 1, 0);
    SetSizer(pSizer);
    Bind(wxEVT_BUTTON, &MyFrame::OpenFileHandeler, this, wxID_FILE);
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
        "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
void MyFrame::OpenFileHandeler(wxCommandEvent& event) {

}
void MyFrame::OnDropFiles(wxDropFilesEvent& event)// Handels the files you drop
{
    if (event.GetNumberOfFiles() > 0) {

        wxString* dropped = event.GetFiles();
        wxASSERT(dropped);

        wxBusyCursor busyCursor;
        wxWindowDisabler disabler;
        // wxBusyInfo busyInfo(_("Adding files, wait please..."));

        wxString name;
        wxArrayString files;

        for (int i = 0; i < event.GetNumberOfFiles(); i++) {
            name = dropped[i];
            if (wxFileExists(name)) {
                files.push_back(name);
            }
            else if (wxDirExists(name)) {
                //  wxDir::GetAllFiles(name, &files);
            }

        }

        wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
        wxASSERT(textCtrl);
        textCtrl->Clear();
        for (size_t i = 0; i < files.size(); i++) {
            *textCtrl << files[i] << wxT('\n');
        }
    }
}

