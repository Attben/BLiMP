#pragma once
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/mediactrl.h>

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
		void OnMuteClick(wxCommandEvent& event);
		void OnVolumeChanged(wxCommandEvent& event);
		void OnTimeChanged(wxCommandEvent& event);
		void OpenFileBrowser(wxCommandEvent& event);
		void OptionsClicked(wxCommandEvent& event);
		void OnListBoxCicked(wxCommandEvent& event);

		//Media events
		void OnMediaFinished(wxMediaEvent& event);
		void OnMediaLoaded(wxMediaEvent& event);
		void OnMediaPause(wxMediaEvent& event);
		void OnMediaPlay(wxMediaEvent& event);
		void OnMediaStop(wxMediaEvent& event);

		void TogglePlayback();

		

		wxBitmap pauseIcon;
		wxBitmap stopIcon;
		wxBitmap playIcon;
		wxBitmap nextIcon;
		wxBitmap rewindIcon;
		wxBitmap openFileIcon;
		wxBitmap muteIcon;
		wxBitmap soundIcon;

		wxMediaCtrl* _mediaPlayer;
		wxPanel* _mediaPanel;
		bool _muted = false;
		int _volumePos;
		double _volume = 1.0;

		

		wxWindowID mediaControlID = wxWindow::NewControlId();
		wxWindowID nextBtnId = wxWindow::NewControlId();
		wxWindowID pauseBtnId = wxWindow::NewControlId();
		wxWindowID previousBtnId = wxWindow::NewControlId();
		wxWindowID settingsBtnId = wxWindow::NewControlId();
		wxWindowID stopBtnId = wxWindow::NewControlId();
		wxWindowID muteButtonID = wxWindow::NewControlId();
		wxWindowID volumeSliderId = wxWindow::NewControlId();
		wxWindowID timeSliderId = wxWindow::NewControlId();
		wxWindowID listBoxId = wxWindow::NewControlId();
		wxWindowID timerId = wxWindow::NewControlId();
		wxCheckBox checkboxAutoplay;


		void OnTimerUpdate(wxTimerEvent& event);
		wxTimer* _timer;
	};
}