// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "../include/AudioSystem.h"
#include <iostream>
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
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    SoundEffect _sound{ "sine440hz.wav" };;
};
enum
{
    ID_Hello = 1
};

wxIMPLEMENT_APP_NO_MAIN(MyApp);
bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Hello World")
{
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
    _sound.play();
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