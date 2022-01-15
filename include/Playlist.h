#pragma once

#include <wx/listctrl.h>

namespace blimp {
	class Playlist : public wxListCtrl {
	public:
		Playlist(wxWindow* parent, wxWindowID id);
		~Playlist();

		void Append(const wxString& item);
		wxString GetItemAt(long index);
		wxString GetNextItem();
		wxString GetPreviousItem();

	private:
		long _currentItem; //base class uses longs instead of size_t for indexing operations.
	};
}