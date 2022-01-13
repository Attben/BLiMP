#include "../include/Playlist.h"
#include <wx/filename.h>

namespace blimp {
	Playlist::Playlist(wxWindow* parent, wxWindowID id)
		: wxListCtrl{ parent, id, wxDefaultPosition,  wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL},
		_currentItem{0L}
	{
		AppendColumn(_("File"), wxLIST_FORMAT_LEFT, /*wxLIST_AUTOSIZE_USEHEADER*/305);
		AppendColumn(_("Length"), wxLIST_FORMAT_CENTER, 75);
	}

	void Playlist::Append(const wxString& path) {
		wxListItem item;
		item.SetAlign(wxLIST_FORMAT_LEFT);
		int ID = GetItemCount();
		item.SetId(ID);
		item.SetMask(wxLIST_MASK_DATA);
		item.SetData(new wxString{ path });
		InsertItem(item);

		this->SetItem(ID, 0, wxFileName(path).GetFullName());
	}

	wxString Playlist::GetNextItem() {
		if (_currentItem < GetItemCount()) {
			return *(wxString*)GetItemData(_currentItem++);
		}
		else {
			return "";
		}
	}

	wxString Playlist::GetPreviousItem() {
		if (_currentItem > 0L) {
			return *(wxString*)GetItemData(_currentItem--);
		}
		else {
			return "";
		}
	}
}