// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
//#include "../include/AudioSystem.h"
//#include <SDL.h>
//#include <SDL_mixer.h>
#include "../include/BLiMP.h"
#include "../include/OptionsWindow.h"
#include <filesystem>
#include <wx/listctrl.h>
#include <wx/mediactrl.h>


namespace blimp {
	wxIMPLEMENT_APP(BlimpApp);
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
		menuFile->Append(settingsBtnId, "Settings");
		menuFile->Append(wxID_EXIT);
		wxMenu* menuHelp = new wxMenu;
		menuHelp->Append(wxID_ABOUT);
		wxMenuBar* menuBar = new wxMenuBar;
		menuBar->Append(menuFile, "&File");
		menuBar->Append(menuHelp, "&Help");
		SetMenuBar(menuBar);
		Bind(wxEVT_MENU, &MainWindow::OnAbout, this, wxID_ABOUT);
		Bind(wxEVT_MENU, &MainWindow::OnExit, this, wxID_EXIT);
		Bind(wxEVT_MENU, &MainWindow::OptionsClicked, this, settingsBtnId);

		//Bind key event handler
		Bind(wxEVT_CHAR_HOOK, &MainWindow::OnKeyDown, this);

		//Init media player
		_mediaPanel = new wxPanel{ this };
#ifdef _WIN32
		//During testing on Windows, wxMediaEvent handling was a bit wonky when using default backend, so we explicitly define WMP10.
		_mediaPlayer = new wxMediaCtrl{ this, mediaControlID, "", wxDefaultPosition, wxDefaultSize, 0, wxMEDIABACKEND_WMP10 };
#else
		_mediaPlayer = new wxMediaCtrl{ this, mediaControlID };
#endif
		_mediaPlayer->ShowPlayerControls();
		Bind(wxEVT_MEDIA_FINISHED, &MainWindow::OnMediaFinished, this);
		Bind(wxEVT_MEDIA_LOADED, &MainWindow::OnMediaLoaded, this);

		//Drag and drop handling
		wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* horizontalFileBox = new wxBoxSizer(wxHORIZONTAL);
		/*wxTextCtrl* dropTarget = new wxTextCtrl(
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
		dropTarget->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainWindow::OnDropFiles), nullptr, this);*/
		_mediaPanel->DragAcceptFiles(true);
		horizontalFileBox->Add(_mediaPanel, 1, wxEXPAND, 0);
		_mediaPanel->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainWindow::OnDropFiles), nullptr, this);

		//Button layout
		wxGridSizer* gs = new wxGridSizer(1, 4, 3, 3);
		openFileIcon.LoadFile("folder-open.png", wxBITMAP_TYPE_PNG);
		wxBitmapButton* fileBtn = new wxBitmapButton(this, wxID_FILE, openFileIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
		fileBtn->SetToolTip("Opens File Explorer to select a file");
		horizontalFileBox->Add(fileBtn);

		pSizer->Add(horizontalFileBox, 1, wxEXPAND);
		wxBoxSizer* horizontalBOX = new wxBoxSizer(wxHORIZONTAL);

		gs = new wxFlexGridSizer(1, 5, 3, 3);

		playicon.LoadFile("play-24.png", wxBITMAP_TYPE_PNG);
		stopIcon.LoadFile("stop-24px.png", wxBITMAP_TYPE_PNG);
		pauseIcon.LoadFile("pause-24px.png", wxBITMAP_TYPE_PNG);
		nextIcon.LoadFile("next-24px.png", wxBITMAP_TYPE_PNG);
		rewindIcon.LoadFile("rewind-24px.png", wxBITMAP_TYPE_PNG);
		wxBitmapButton* previousBtn = new wxBitmapButton(this, previousBtnId, rewindIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Previous");
		wxBitmapButton* PauseBtn = new wxBitmapButton(this, pauseBtnId, playicon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Pause");
		wxBitmapButton* nextBtn = new wxBitmapButton(this, nextBtnId, nextIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Next");
		wxBitmapButton* stopBtn = new wxBitmapButton(this, stopBtnId, stopIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "stop");
		wxSlider* volumeSlider = new wxSlider(this, volumeSliderId, 10, 0, 10, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "VolumeSlider");
		previousBtn->SetToolTip("Plays the previous file");
		PauseBtn->SetToolTip("Click to toggle between play and pause");
		nextBtn->SetToolTip("Plays the next file in the list");
		stopBtn->SetToolTip("Stops and resets the file to the beginning");
		volumeSlider->SetToolTip("Changes the volume of the aplication. Left side is lowest volume, right side is loudest volume");
		gs->Add(previousBtn);
		gs->Add(PauseBtn);
		gs->Add(nextBtn);
		gs->Add(stopBtn);
		gs->Add(volumeSlider);

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
		Bind(wxEVT_SLIDER, &MainWindow::OnVolumeChanged, this, volumeSliderId);
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
					files.push_back(name);
				}
				else if (wxDirExists(name)) {
					//  wxDir::GetAllFiles(name, &files);
				}
			}
			_mediaPlayer->Load(files.back());

			/*wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
			wxASSERT(textCtrl);
			textCtrl->Clear();
			for (size_t i = 0; i < files.size(); i++) {
				*textCtrl << files[i] << wxT('\n');
			}*/
		}
	}

	/*
	*Key event handler
	*/
	void MainWindow::OnKeyDown(wxKeyEvent& event) {
		switch (event.GetUnicodeKey()) {
		case WXK_SPACE:
			TogglePlayback();
			break;
		default:
			event.Skip();
			break;
		}
	}

	void MainWindow::OnMediaFinished(wxMediaEvent& event) {
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(playicon);
	}

	void MainWindow::OnMediaLoaded(wxMediaEvent& event) {
		_mediaPlayer->Play();
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(pauseIcon);
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
		TogglePlayback();
	}

	void MainWindow::OnPreviousClick(wxCommandEvent& event)
	{
		//NYI
	}

	void MainWindow::OnStopClick(wxCommandEvent& event) {
		_mediaPlayer->Stop();
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(playicon);
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
			//add to playlist
		}
		_mediaPlayer->Load(paths.back());
	}

	void MainWindow::OptionsClicked(wxCommandEvent& event)
	{
		Optionswindow* frame = new Optionswindow();
		frame->Show(true);
	}
  
	void MainWindow::OnVolumeChanged(wxCommandEvent& event)
	{
		wxSlider* slider = wxDynamicCast(FindWindow(volumeSliderId), wxSlider);
		slider->GetValue();
	}
}

	void MainWindow::TogglePlayback() {
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		switch (_mediaPlayer->GetState()) {

		case wxMEDIASTATE_PLAYING:
			_mediaPlayer->Pause();
			button->SetBitmapLabel(playicon);
			break;
		default:
			_mediaPlayer->Play();
			button->SetBitmapLabel(pauseIcon);
			break;
		}
	}
}

//int main(int argc, char** argv) {
//	//TODO: Don't hardcode audio parameters.
//	constexpr int SAMPLE_RATE = 44100;
//	constexpr int CHANNELS = 2;
//	constexpr int CHUNK_SIZE = 2048;
//
//	//Init SDL2 systems
//	if (SDL_Init(SDL_INIT_AUDIO) < 0)
//	{
//		std::cerr << "Could not initialize SDL.\n";
//		return 1;
//	}
//	if (Mix_OpenAudio(SAMPLE_RATE, MIX_DEFAULT_FORMAT, CHANNELS, CHUNK_SIZE) < 0)
//	{
//		std::cerr << "SDL_mixer could not initialize!" <<
//			"SDL_mixer Error : " << Mix_GetError();
//		return 1;
//	}
//
//	//Run main window
//	wxEntry(argc, argv);
//	//Cleanup
//
//	Mix_Quit();
//	SDL_Quit();
//	return 0;
//}
