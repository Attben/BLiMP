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
		void OnMediaLoaded(wxMediaEvent& event);
		void OnNextClick(wxCommandEvent& event);
		void OnPauseClick(wxCommandEvent& event);
		void OnPreviousClick(wxCommandEvent& event);
		void OnStopClick(wxCommandEvent& event);
		void OpenFileBrowser(wxCommandEvent& event);
		void OptionsClicked(wxCommandEvent& event);

		void TogglePlayback();

		wxBitmap pauseIcon;
		wxBitmap stopIcon;
		wxBitmap playicon;
		wxBitmap nextIcon;
		wxBitmap rewindIcon;
		wxBitmap openFileIcon;

		wxMediaCtrl* _mediaPlayer;
		wxPanel* _mediaPanel;

		wxWindowID mediaControlID = wxWindow::NewControlId();
		wxWindowID nextBtnId = wxWindow::NewControlId();
		wxWindowID pauseBtnId = wxWindow::NewControlId();
		wxWindowID previousBtnId = wxWindow::NewControlId();
		wxWindowID settingsBtnId = wxWindow::NewControlId();
		wxWindowID stopBtnId = wxWindow::NewControlId();

		wxCheckBox checkboxAutoplay;
	};
}