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

		//Drag and drop handling
		_mediaPlayer->DragAcceptFiles(true);
		_mediaPlayer->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainWindow::OnDropFiles), nullptr, this);

		//Load button icons
		openFileIcon.LoadFile("folder-open.png", wxBITMAP_TYPE_PNG);
		playIcon.LoadFile("play-24.png", wxBITMAP_TYPE_PNG);
		stopIcon.LoadFile("stop-24px.png", wxBITMAP_TYPE_PNG);
		pauseIcon.LoadFile("pause-24px.png", wxBITMAP_TYPE_PNG);
		nextIcon.LoadFile("next-24px.png", wxBITMAP_TYPE_PNG);
		rewindIcon.LoadFile("rewind-24px.png", wxBITMAP_TYPE_PNG);
		muteIcon.LoadFile("mute-24px.png", wxBITMAP_TYPE_PNG);
		soundIcon.LoadFile("sound-24px.png", wxBITMAP_TYPE_PNG);
		

		//Create UI elements
		wxBitmapButton* fileButton = new wxBitmapButton(this, wxID_FILE, openFileIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
		wxBitmapButton* previousButton = new wxBitmapButton(this, previousBtnId, rewindIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Previous");
		wxBitmapButton* playbackButton = new wxBitmapButton(this, pauseBtnId, playIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Pause");
		wxBitmapButton* nextButton = new wxBitmapButton(this, nextBtnId, nextIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Next");
		wxBitmapButton* stopButton = new wxBitmapButton(this, stopBtnId, stopIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "stop");
		wxBitmapButton* muteButton = new wxBitmapButton(this, muteButtonID, soundIcon);
		wxSlider* volumeSlider = new wxSlider(this, volumeSliderId, 10, 0, 10, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "VolumeSlider");
		wxSlider* timeSlider = new wxSlider(this, timeSliderId, 0, 0, 100, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "VolumeSlider");
		previousButton->SetToolTip("Plays the previous file");
		playbackButton->SetToolTip("Click to toggle between play and pause");
		nextButton->SetToolTip("Plays the next file in the list");
		stopButton->SetToolTip("Stops and resets the file to the beginning");
		volumeSlider->SetToolTip("Changes the volume of the aplication. Left side is lowest volume, right side is loudest volume");
		fileButton->SetToolTip("Opens File Explorer to select a file");

		//Create sizer objects
		wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* horizontalFileBox = new wxBoxSizer(wxHORIZONTAL);
		wxBoxSizer* timeBox = new wxBoxSizer(wxHORIZONTAL);
		wxFlexGridSizer* gs = new wxFlexGridSizer(1, 0, 3, 3);
		wxBoxSizer* playbackControlsBox = new wxBoxSizer(wxHORIZONTAL);

		//Add UI elements to sizers
		timeBox->Add(timeSlider);
		horizontalFileBox->Add(_mediaPlayer, 1, wxEXPAND, 0);
		horizontalFileBox->Add(fileButton);
		gs->Add(previousButton);
		gs->Add(playbackButton);
		gs->Add(nextButton);
		gs->Add(stopButton);
		gs->Add(3 * playIcon.GetWidth(), 0);
		gs->Add(muteButton);
		gs->Add(volumeSlider);

		playbackControlsBox->Add(gs, 1, wxEXPAND);
		pSizer->Add(horizontalFileBox, 1, wxEXPAND);
		pSizer->Add(timeBox, 0, wxEXPAND);
		pSizer->Add(playbackControlsBox, 0, wxEXPAND);
		SetSizer(pSizer);
		
		wxSize displaySize = wxGetDisplaySize();
		
		SetMinSize(wxSize(270, 220));
		SetMaxSize(displaySize);
		Centre();

		//Bind button events
		Bind(wxEVT_BUTTON, &MainWindow::OpenFileBrowser, this, wxID_FILE);
		Bind(wxEVT_BUTTON, &MainWindow::OnPreviousClick, this, previousBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnPauseClick, this, pauseBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnNextClick, this, nextBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnStopClick, this, stopBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnMuteClick, this, muteButtonID);
		Bind(wxEVT_SLIDER, &MainWindow::OnVolumeChanged, this, volumeSliderId);
		Bind(wxEVT_SLIDER, &MainWindow::OnTimeChanged, this, timeSliderId);

		//Bind media events
		Bind(wxEVT_MEDIA_FINISHED, &MainWindow::OnMediaFinished, this);
		Bind(wxEVT_MEDIA_LOADED, &MainWindow::OnMediaLoaded, this);
		Bind(wxEVT_MEDIA_PAUSE, &MainWindow::OnMediaPause, this);
		Bind(wxEVT_MEDIA_PLAY, &MainWindow::OnMediaPlay, this);
		Bind(wxEVT_MEDIA_STOP, &MainWindow::OnMediaStop, this);
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
		button->SetBitmapLabel(playIcon);
	}

	void MainWindow::OpenFileBrowser(wxCommandEvent& event) {
		wxFileDialog openFileDialog{
			this, //parent
			_("Open sound file"), //Title
			std::filesystem::current_path().c_str(), //Default directory
			"", //Default file
			"Sound files (*.wav;*.flac;*.mid;*.mp3;*.ogg)|*.wav;*.flac;*.mid;*.mp3;*.ogg|\
			Video files(*.mkv;*.mov;*.mp4;*.wmv)|*.mkv;*.mov;*.mp4;*.wmv|\
			All files (*)|*",
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

	void MainWindow::OnMuteClick(wxCommandEvent& event) {
		wxButton* muteButton = wxDynamicCast(FindWindow(muteButtonID), wxButton);
		wxSlider* slider = wxDynamicCast(FindWindow(volumeSliderId), wxSlider);
		int sliderWidth = (slider->GetMax() - slider->GetMin());

		if (_muted) {
			_mediaPlayer->SetVolume(_volume); //Reload previous value
			slider->SetValue(_volumePos);
			muteButton->SetBitmapLabel(soundIcon);
		}
		else {
			_volume = _mediaPlayer->GetVolume(); //Save previous value
			_mediaPlayer->SetVolume(0.0);
			muteButton->SetBitmapLabel(muteIcon);
			_volumePos = slider->GetValue();
			slider->SetValue(0);
		}
		_muted = !_muted;
	}
  
	void MainWindow::OnVolumeChanged(wxCommandEvent& event)
	{
		wxSlider* slider = wxDynamicCast(FindWindow(volumeSliderId), wxSlider);
		wxButton* muteButton = wxDynamicCast(FindWindow(muteButtonID), wxButton);
		int volumePos = slider->GetValue();
		double fractionOfMaxVolumePos = (double)volumePos / ((double)slider->GetMin() + slider->GetMax());
		double desiredVolume = fractionOfMaxVolumePos * volumePos;
		_mediaPlayer->SetVolume(desiredVolume);

		if (volumePos > slider->GetMin()) {
			_volume = desiredVolume; //Save these for later use with the mute mutton
			_volumePos = volumePos;//
			muteButton->SetBitmapLabel(soundIcon);
			_muted = false;
		}
		else {
			muteButton->SetBitmapLabel(muteIcon);
			_muted = true;

		}
	}

	void MainWindow::OnTimeChanged(wxCommandEvent& event)
	{
		wxSlider* slider = wxDynamicCast(FindWindow(timeSliderId), wxSlider);
		wxFileOffset mediaLength = _mediaPlayer->Length();
		double incrementsOfTime = 100.0 / mediaLength;
		int chosenTime = slider->GetValue();
		double wantedTime = chosenTime * incrementsOfTime;
		_mediaPlayer->Seek(wantedTime, wxFromStart);
	}

	/*
	*Media event handlers
	*/
	void MainWindow::OnMediaFinished(wxMediaEvent& event) {
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(playIcon);
	}

	void MainWindow::OnMediaLoaded(wxMediaEvent& event) {
		_mediaPlayer->Play();
		Layout();
	}

	void MainWindow::OnMediaPause(wxMediaEvent& event) {
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(playIcon);
	}

	void MainWindow::OnMediaPlay(wxMediaEvent& event) {
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(pauseIcon);
	}

	void MainWindow::OnMediaStop(wxMediaEvent& event) {
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(playIcon);
	}

	void MainWindow::TogglePlayback() {
		switch (_mediaPlayer->GetState()) {

		case wxMEDIASTATE_PLAYING:
			_mediaPlayer->Pause();
			break;
		default:
			_mediaPlayer->Play();
			break;
		}
	}
}
