// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "../include/AudioSystem.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include "../Gridsizer.h"

namespace blimp {
	class BlimpApp : public wxApp {
	public:
		virtual bool OnInit();
	};

	class MainWindow : public wxFrame {
	public:
		MainWindow();

	private:
		//Menu item handlers
		void OnAbout(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);

		//Event handlers
		void OnDropFiles(wxDropFilesEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnNextClick(wxCommandEvent& event);
		void OnPauseClick(wxCommandEvent& event);
		void OnPreviousClick(wxCommandEvent& event);
		void OnStopClick(wxCommandEvent& event);
		void OpenFileBrowser(wxCommandEvent& event);

		AudioSystem _audioSystem;
	};

	wxIMPLEMENT_APP_NO_MAIN(BlimpApp);
	bool BlimpApp::OnInit() {
		wxInitAllImageHandlers();
		MainWindow* frame = new MainWindow();
		frame->Show(true);
		return true;
	}

	MainWindow::MainWindow()
		: wxFrame(nullptr, wxID_ANY, "BLiMP") {
		//Implement menu bar
		wxMenu* menuFile = new wxMenu;
		menuFile->AppendSeparator();
		menuFile->Append(wxID_EXIT);
		wxMenu* menuHelp = new wxMenu;
		menuHelp->Append(wxID_ABOUT);
		wxMenuBar* menuBar = new wxMenuBar;
		menuBar->Append(menuFile, "&File");
		menuBar->Append(menuHelp, "&Help");
		SetMenuBar(menuBar);
		Bind(wxEVT_MENU, &MainWindow::OnAbout, this, wxID_ABOUT);
		Bind(wxEVT_MENU, &MainWindow::OnExit, this, wxID_EXIT);

		//Bind key event handler
		Bind(wxEVT_CHAR_HOOK, &MainWindow::OnKeyDown, this);

		//Drag and drop handling
		wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* horizontalFileBox = new wxBoxSizer(wxHORIZONTAL);
		wxTextCtrl* dropTarget = new wxTextCtrl(
			this,
			wxID_ANY,
			_("Drop files onto me!"),
			wxDefaultPosition,
			wxDefaultSize,
			wxTE_MULTILINE | wxTE_READONLY
		);
		dropTarget->DragAcceptFiles(true);
		horizontalFileBox->Add(dropTarget, 1, wxEXPAND, 0);
		Layout();
		Centre();
		dropTarget->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainWindow::OnDropFiles), nullptr, this);

		//Button layout
		wxGridSizer* gs = new wxGridSizer(1, 4, 3, 3);
		wxButton* fileBtn = new wxButton(this, wxID_FILE, "OpenFileBtn", wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
		horizontalFileBox->Add(fileBtn);

		pSizer->Add(horizontalFileBox, 1, wxEXPAND);
		wxBoxSizer* horizontalBOX = new wxBoxSizer(wxHORIZONTAL);

		gs = new wxFlexGridSizer(1, 4, 3, 3);

		wxWindowID previousBtnId = wxWindow::NewControlId();
		wxWindowID pauseBtnId = wxWindow::NewControlId();
		wxWindowID nextBtnId = wxWindow::NewControlId();
		wxWindowID stopBtnId = wxWindow::NewControlId();
		gs->Add(new wxButton(this, previousBtnId, "Previous", wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Previous"));
		gs->Add(new wxButton(this, pauseBtnId, " Play  ", wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Pause"));
		gs->Add(new wxButton(this, nextBtnId, "Next", wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Next"));
		gs->Add(new wxButton(this, stopBtnId, "stop", wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "stop"));

		wxBitmap rsswa;
		rsswa.LoadFile("player_play.png",wxBITMAP_TYPE_PNG);
		gs->Add(new wxBitmapButton(this, -1, rsswa, wxPoint(10, 10), wxSize(32, 32), 0));
		horizontalBOX->Add(gs, 1, wxEXPAND);

		horizontalBOX->Add(gs, 1, wxEXPAND);
		pSizer->Add(horizontalBOX, 1, wxEXPAND);
		SetSizer(pSizer);
		SetMinSize(wxSize(270, 220));

		Centre();

		Bind(wxEVT_BUTTON, &MainWindow::OpenFileBrowser, this, wxID_FILE);
		Bind(wxEVT_BUTTON, &MainWindow::OnPreviousClick, this, previousBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnPauseClick, this, pauseBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnNextClick, this, nextBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnStopClick, this, stopBtnId);
	}

	/*
	*Menu items
	*/
	void MainWindow::OnAbout(wxCommandEvent& event) {
		wxMessageBox("This is a wxWidgets Hello World example",
			"About Hello World", wxOK | wxICON_INFORMATION);
	}

	void MainWindow::OnExit(wxCommandEvent& event) {
		Close(true);
	}

	/*
	*Drag-and-drop handler
	*/
	void MainWindow::OnDropFiles(wxDropFilesEvent& event) {
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

	/*
	*Key event handler
	*/
	void MainWindow::OnKeyDown(wxKeyEvent& event) {
		switch (event.GetUnicodeKey()) {
		case WXK_SPACE:
			_audioSystem.togglePlayback();
			break;
		default:
			event.Skip();
			break;
		}
	}

	/*
	*Button click handlers
	*/
	void MainWindow::OnNextClick(wxCommandEvent& event)
	{
		//NYI
	}

	void MainWindow::OnPauseClick(wxCommandEvent& event)
	{
		_audioSystem.togglePlayback();
		wxWindowID dssa = event.GetId();
		wxButton* button = wxDynamicCast(FindWindow(dssa), wxButton);
		if (Mix_PausedMusic()) {

			if (button) button->SetLabel(wxT("Play"));
		}
		else if (Mix_PlayingMusic()) {
			if (button) button->SetLabel(wxT("Pause"));
		}
		else {
			if (button) button->SetLabel(wxT("Play"));
		}
	}

	void MainWindow::OnPreviousClick(wxCommandEvent& event)
	{
		//NYI
	}

	void MainWindow::OnStopClick(wxCommandEvent& event) {
		_audioSystem.stop();
	}

	void MainWindow::OpenFileBrowser(wxCommandEvent& event) {
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
