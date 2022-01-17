// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

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

		_volumePos = volumeSlider->GetMax();

		_playlist = new Playlist{ this, wxID_ANY };

		//Create sizer objects
		wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* verticalInsideHorizontalbox = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* horizontalFileBox = new wxBoxSizer(wxHORIZONTAL);
		wxBoxSizer* timeBox = new wxBoxSizer(wxHORIZONTAL);
		wxFlexGridSizer* gs = new wxFlexGridSizer(1, 0, 3, 3);
		wxBoxSizer* playbackControlsBox = new wxBoxSizer(wxHORIZONTAL);

		//Add UI elements to sizers
		_playlist->SetColumnWidth(0, 200);
		_playlist->SetForegroundColour(BACKGROUND_GREEN);
		verticalInsideHorizontalbox->Add(fileButton);
		verticalInsideHorizontalbox->Add(_playlist);
		timeBox->Add(timeSlider);
		horizontalFileBox->Add(_mediaPlayer, 1, wxEXPAND, 0);
		horizontalFileBox->Add(verticalInsideHorizontalbox, 0, wxEXPAND);

		gs->Add(previousButton);
		gs->Add(playbackButton);
		gs->Add(nextButton);
		gs->Add(stopButton);
		gs->Add(3 * playIcon.GetWidth(), 0); //Add empty space
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
		_timer = new wxTimer(this, timerId);
		//Bind button events
		Bind(wxEVT_BUTTON, &MainWindow::OpenFileBrowser, this, wxID_FILE);
		Bind(wxEVT_BUTTON, &MainWindow::OnPreviousClick, this, previousBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnPauseClick, this, pauseBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnNextClick, this, nextBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnStopClick, this, stopBtnId);
		Bind(wxEVT_BUTTON, &MainWindow::OnMuteClick, this, muteButtonID);
		Bind(wxEVT_SLIDER, &MainWindow::OnVolumeChanged, this, volumeSliderId);
		Bind(wxEVT_SLIDER, &MainWindow::OnTimeChanged, this, timeSliderId);
		Bind(wxEVT_SLIDER, &MainWindow::OnPlaylistDoubleclick, this, listBoxId);

		//Bind media events
		Bind(wxEVT_MEDIA_FINISHED, &MainWindow::OnMediaFinished, this);
		Bind(wxEVT_MEDIA_LOADED, &MainWindow::OnMediaLoaded, this);
		Bind(wxEVT_MEDIA_PAUSE, &MainWindow::OnMediaPause, this);
		Bind(wxEVT_MEDIA_PLAY, &MainWindow::OnMediaPlay, this);
		Bind(wxEVT_MEDIA_STOP, &MainWindow::OnMediaStop, this);

		Bind(wxEVT_TIMER, &MainWindow::OnTimerUpdate, this, timerId);
		Bind(wxEVT_LIST_ITEM_ACTIVATED, &MainWindow::OnPlaylistDoubleclick, this);
	}

	MainWindow::~MainWindow() {
		delete _timer;
	}

	/*
	*Menu items
	*/
	void MainWindow::OnAbout(wxCommandEvent& event) {
		wxMessageBox("BLiMP (Bad Lightweight Media Player) version 0.42 alpha",
			"About BLiMP", wxOK | wxICON_INFORMATION);
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

			for (int i = 0; i < event.GetNumberOfFiles(); i++) {
				wxString fileName = dropped[i];
				if (wxFileExists(fileName)) {
					_playlist->Append(fileName);
				}
				else if (wxDirExists(fileName)) {
					//  wxDir::GetAllFiles(name, &files);
				}
			}

			const wxString nextItem = _playlist->GetNextItem();
			if (nextItem != "") {
				_mediaPlayer->Load(nextItem);
			}
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
		wxString filePath = _playlist->GetNextItem();
		if (filePath != "") {
			_mediaPlayer->Load(filePath);
		}
	}

	void MainWindow::OnPauseClick(wxCommandEvent& event)
	{
		TogglePlayback();
	}

	void MainWindow::OnPreviousClick(wxCommandEvent& event)
	{
		wxString filePath = _playlist->GetPreviousItem();
		if (filePath != "") {
			_mediaPlayer->Load(filePath);
		}
	}

	void MainWindow::OnStopClick(wxCommandEvent& event) {
		_mediaPlayer->Stop();
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		button->SetBitmapLabel(playIcon);
	}

	void MainWindow::OpenFileBrowser(wxCommandEvent& event) {
		wxFileDialog openFileDialog{
			this, //parent
			_("Open media file"), //Title
			std::filesystem::current_path().c_str(), //Default directory
			"", //Default file
			"Sound files (*.wav;*.flac;*.mid;*.mp3;)|*.wav;*.flac;*.mid;*.mp3;|\
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
			_playlist->Append(path);
		}
		_mediaPlayer->Load(_playlist->GetNextItem());
	}

	void MainWindow::OptionsClicked(wxCommandEvent& event)
	{
		Optionswindow* frame = new Optionswindow();
		frame->Show(true);
	}

	void MainWindow::OnPlaylistDoubleclick(wxCommandEvent& event)
	{
		//The compiler doesn't seem to want to Bind() wxEVT_LIST_ITEM_ACTIVATED
		//to a function that takes an actual wxListEvent& as its argument, so
		//this ugly (and probably really sketchy) typecast is used as a workaround.
		auto listEvent = dynamic_cast<wxListEvent*>(&event);
		wxString path = _playlist->GetItemAt(listEvent->m_itemIndex);
		if (path != "") {
			_mediaPlayer->Load(path);
		}
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
		int sliderMin = slider->GetMin();
		int sliderMax = slider->GetMax();
		int volumePos = slider->GetValue();
		double desiredVolume = ((double)volumePos - sliderMin) / ((double)sliderMax - sliderMin); //Fraction of max volume position
		_mediaPlayer->SetVolume(desiredVolume);

		if (volumePos > sliderMin) {
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
		wxFileOffset incrementsOfTime = (mediaLength / (slider->GetMax() - slider->GetMin()));
		wxFileOffset chosenTime = slider->GetValue();
		wxFileOffset wantedTime = chosenTime * incrementsOfTime;
		_mediaPlayer->Seek(wantedTime, wxFromStart);

	}

	/*
	*Media event handlers
	*/
	void MainWindow::OnMediaFinished(wxMediaEvent& event) {
		wxString nextFile = _playlist->GetNextItem();
		if (nextFile == "") { //Did we reach the end of the playlist?
			wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
			button->SetBitmapLabel(playIcon);
			_timer->Stop();
		}
		else {
			_mediaPlayer->Load(nextFile);
		}
	}

	void MainWindow::OnMediaLoaded(wxMediaEvent& event) {
		_mediaPlayer->Play();
		wxSlider* slider = wxDynamicCast(FindWindow(timeSliderId), wxSlider);
		slider->SetValue(0);
		Layout();
		_timer->Start(1000);
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
			_timer->Stop();
			break;
		default:
			_mediaPlayer->Play();
			_timer->Start(1000);
			break;
		}
	}
	void MainWindow::OnTimerUpdate(wxTimerEvent& event)
	{
		wxSlider* slider = wxDynamicCast(FindWindow(timeSliderId), wxSlider);
		wxFileOffset mediaLength = _mediaPlayer->Length();
		wxFileOffset currentPosition = _mediaPlayer->Tell();
		double incrementsOfTime = (mediaLength / (slider->GetMax() - slider->GetMin()));
		double slidervalue = (currentPosition / incrementsOfTime);
		slider->SetValue(slidervalue);

	}
}
