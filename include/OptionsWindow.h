#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <iostream>
#include <fstream>
namespace blimp {
	class Optionswindow :public wxFrame {
	public:
		Optionswindow();

	private:
		wxWindowID autoplayID = wxWindow::NewControlId();
		void AutoplayCheckboxClicked(wxCommandEvent& event);
		void write(bool input);
		bool read();
	};
}