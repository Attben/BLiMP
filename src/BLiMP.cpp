// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "../include/AudioSystem.h"
#include <SDL.h>
#include <SDL_mixer.h>
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
    //Event handlers
    void OpenFileHandler(wxCommandEvent& event);
    void OnDropFiles(wxDropFilesEvent& event);
    void OnPauseClick(wxCommandEvent& event);
    void OnNextClick(wxCommandEvent& event);
    void OnPreviousClick(wxCommandEvent& event);
    void OnStopClick(wxCommandEvent& event);

	blimp::AudioSystem _audioSystem;
};
enum
{
    ID_Hello = 1
};
wxIMPLEMENT_APP_NO_MAIN (MyApp);
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
    fileBtn->SetSize(10, 4);
    pSizer->Add(fileBtn, 1, 0);
    SetSizer(pSizer);

   wxWindowID previousBtnId = wxWindow::NewControlId();
   wxWindowID pauseBtnId = wxWindow::NewControlId();
   wxWindowID nextBtnId = wxWindow::NewControlId();
    wxButton* previousBtn = new wxButton(this, previousBtnId, "Previous", wxPoint(2, 9), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
    fileBtn->SetSize(10, 4);
    pSizer->Add(previousBtn, 1, 1);
    SetSizer(pSizer);
    wxButton* pauseBtn = new wxButton(this, pauseBtnId, "Pause", wxPoint(2, 10), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
    fileBtn->SetSize(10, 4);
    pSizer->Add(pauseBtn, 1, 2);
    SetSizer(pSizer);
    wxButton* nextBtn = new wxButton(this, nextBtnId, "Next", wxPoint(2, 10), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
    fileBtn->SetSize(10, 4);
    pSizer->Add(nextBtn, 1, 3);
    SetSizer(pSizer);
    Bind(wxEVT_BUTTON, &MyFrame::OpenFileHandler, this, wxID_FILE);
    Bind(wxEVT_BUTTON, &MyFrame::OnPreviousClick, this, wxID_FILE1);
    Bind(wxEVT_BUTTON, &MyFrame::OnPauseClick, this, wxID_FILE2);
    Bind(wxEVT_BUTTON, &MyFrame::OnNextClick, this, wxID_FILE3);
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
    wxLogMessage("Hello world from wxWidgets and SDL2_mixer!");
	_audioSystem.playFile(0);
}

void MyFrame::OpenFileHandler(wxCommandEvent& event)// Handles the btn
{
    
    wxFileDialog openFileDialog{
        this, //parent
        _("Open sound file"), //Title
        std::filesystem::current_path().c_str(), //Default directory
        "", //Default file
        "Sound files (*.wav;*.mp3;*.flac;*.ogg)|*.wav;*.mp3;*.flac;*.ogg",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE
    };
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    wxArrayString paths;
    openFileDialog.GetPaths(paths);
    for (wxString path : paths) {
        _audioSystem.addFile(path.ToStdString());
    }
}
void MyFrame::OnDropFiles(wxDropFilesEvent& event)// Handles the files you drop
{
    if (event.GetNumberOfFiles() > 0) {

        wxString* dropped = event.GetFiles();
        wxASSERT(dropped);

        wxBusyCursor busyCursor;
        wxWindowDisabler disabler;
        // wxBusyInfo busyInfo(_("Adding files, wait please..."));

        std::string name;
        wxArrayString files;

        for (int i = 0; i < event.GetNumberOfFiles(); i++) {
            name = dropped[i];
            if (wxFileExists(name)) {
                _audioSystem.addFile(name);
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

void MyFrame::OnPauseClick(wxCommandEvent& event)
{
    _audioSystem.togglePlayback();
}

void MyFrame::OnNextClick(wxCommandEvent& event)
{
}

void MyFrame::OnPreviousClick(wxCommandEvent& event)
{
}

void MyFrame::OnStopClick(wxCommandEvent& event) {
    _audioSystem.stop();
}

int main(int argc, char** argv) {
    //TODO: Don't hardcode audio parameters.
    constexpr int SAMPLE_RATE = 44100;
    constexpr int CHANNELS = 2;
    constexpr int CHUNK_SIZE = 2048;

    //Init SDL2 systems
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "Could not initialize SDL.\n";
        return 1;
    }
    if (Mix_OpenAudio(SAMPLE_RATE, MIX_DEFAULT_FORMAT, CHANNELS, CHUNK_SIZE) < 0)
    {
        std::cerr << "SDL_mixer could not initialize!" <<
            "SDL_mixer Error : " << Mix_GetError();
        return 1;
    }

    //Run main window
    wxEntry(argc, argv);
    //Cleanup

    Mix_Quit();
    SDL_Quit();
    return 0;
}
