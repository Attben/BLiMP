// wxWidgets "Hello World" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "../include/AudioSystem.h"
#include <SDL.h>
#include <SDL_mixer.h>


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
		void OptionsClicked(wxCommandEvent& event);

		AudioSystem _audioSystem;
		wxBitmap pauseIcon;
		wxBitmap stopIcon;
		wxBitmap playicon;
		wxBitmap nextIcon;
		wxBitmap rewindIcon;
		wxBitmap openFileIcon;

		wxWindowID settingsBtnId = wxWindow::NewControlId();
		wxWindowID previousBtnId = wxWindow::NewControlId();
		wxWindowID pauseBtnId = wxWindow::NewControlId();
		wxWindowID nextBtnId = wxWindow::NewControlId();
		wxWindowID stopBtnId = wxWindow::NewControlId();

		wxCheckBox checkboxAutoplay;
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
		menuFile->Append(settingsBtnId,"Settings");
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

		//Drag and drop handling
		wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* horizontalFileBox = new wxBoxSizer(wxHORIZONTAL);
		wxTextCtrl* dropTarget = new wxTextCtrl(
			this,
			wxID_ANY,
			_("Drop files onto me!"),
			wxDefaultPosition,
			wxDefaultSize,
			wxTE_MULTILINE | wxTE_READONLY);
		dropTarget->DragAcceptFiles(true);
		horizontalFileBox->Add(dropTarget, 1, wxEXPAND, 0);
		Layout();
		Centre();
		dropTarget->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(MainWindow::OnDropFiles), nullptr, this);

		//Button layout
		wxGridSizer* gs = new wxGridSizer(1, 4, 3, 3);
		openFileIcon.LoadFile("folder-open.png", wxBITMAP_TYPE_PNG);
		wxBitmapButton* fileBtn = new wxBitmapButton(this, wxID_FILE, openFileIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "FileOpener");
		fileBtn->SetToolTip("Opens File Explorer to select a file");
		horizontalFileBox->Add(fileBtn);

		pSizer->Add(horizontalFileBox, 1, wxEXPAND);
		wxBoxSizer* horizontalBOX = new wxBoxSizer(wxHORIZONTAL);

		gs = new wxFlexGridSizer(1, 4, 3, 3);


		
		playicon.LoadFile("play-24.png",wxBITMAP_TYPE_PNG);
		stopIcon.LoadFile("stop-24px.png", wxBITMAP_TYPE_PNG);
		pauseIcon.LoadFile("pause-24px.png", wxBITMAP_TYPE_PNG);
		nextIcon.LoadFile("next-24px.png", wxBITMAP_TYPE_PNG);
		rewindIcon.LoadFile("rewind-24px.png", wxBITMAP_TYPE_PNG);
		wxBitmapButton* previousBtn = new wxBitmapButton(this, previousBtnId, rewindIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Previous");
		wxBitmapButton* PauseBtn = new wxBitmapButton(this, pauseBtnId, playicon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Pause");
		wxBitmapButton* nextBtn = new wxBitmapButton(this, nextBtnId,nextIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "Next");
		wxBitmapButton* stopBtn = new wxBitmapButton(this, stopBtnId, stopIcon, wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "stop");

		previousBtn->SetToolTip("Plays the previous file");
		PauseBtn->SetToolTip("Click to toggle between play and pause");
		nextBtn->SetToolTip("Plays the next file in the list");
		stopBtn->SetToolTip("Stops and resets the file to the beginning");


		gs->Add(previousBtn);
		gs->Add(PauseBtn);
		gs->Add(nextBtn);
		gs->Add(stopBtn);
		
		
		

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
		wxButton* button = wxDynamicCast(FindWindow(pauseBtnId), wxButton);
		if (Mix_PausedMusic()) {

			 button->SetBitmapLabel(playicon);
		}
		else if (Mix_PlayingMusic()) {
			 button->SetBitmapLabel(pauseIcon);
		}
		else {
			button->SetBitmapLabel(playicon);
		}
	}

	void MainWindow::OnPreviousClick(wxCommandEvent& event)
	{
		//NYI
	}

	void MainWindow::OnStopClick(wxCommandEvent& event) {
		_audioSystem.stop();
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
			_audioSystem.addFile(path.ToStdString());
		}
	}

	void MainWindow::OptionsClicked(wxCommandEvent& event)
	{
		Optionswindow* frame;
		frame->Show(true);
	}
	
	class Optionswindow :public wxFrame{
	public:
		Optionswindow();

	private:
		wxWindowID autoplayID = wxWindow::NewControlId();

	};

	Optionswindow::Optionswindow():wxFrame(nullptr, wxID_ANY, "Options")
	{
		wxCheckBox *autoplayCheckbox = new wxCheckBox(this,autoplayID,"Autoplay",wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "autoplayOption");

		Show();
	}

	
}
void write()
{
	//NYI
}
bool read()
{
	//NYI
	return true;
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
