#include "../include/Playlist.h"

namespace blimp {
	Playlist::Playlist(wxWindow* parent, wxWindowID id)
		: wxListCtrl{ parent, id },
		_currentItem{0L}
	{}

	void Playlist::Append(const wxString& item) {
		wxListItem newItem;
		newItem.SetAlign(wxLIST_FORMAT_LEFT);
		int ID = this->GetItemCount();
		newItem.SetId(ID);
		newItem.SetData(new wxString{ item });
		InsertItem(newItem);
	}

	wxString Playlist::GetNextItem() {
		if (_currentItem >= GetTopItem()) {
			return "";
		}
		else {
			return GetItemText(++_currentItem);
		}
	}
}