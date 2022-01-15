#include "../include/Playlist.h"
#include <wx/filename.h>

namespace blimp {
	Playlist::Playlist(wxWindow* parent, wxWindowID id)
		: wxListCtrl{ parent, id, wxDefaultPosition,  wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL },
		_currentItem{ -1L }
	{
		AppendColumn(_("File"), wxLIST_FORMAT_LEFT, /*wxLIST_AUTOSIZE_USEHEADER*/305);
		//AppendColumn(_("Length"), wxLIST_FORMAT_CENTER, 75);
	}

	Playlist::~Playlist() {
		for (long n = 0L; n < GetItemCount(); ++n) {
			wxString* dataPtr = (wxString*)GetItemData(n);
			delete dataPtr;
		}
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

	wxString Playlist::GetItemAt(long index) {
		if (index < 0L || index >= GetItemCount()) {
			return "";
		}
		else {
			_currentItem = index;
			return *(wxString*)GetItemData(_currentItem);
		}
	}

	wxString Playlist::GetNextItem() {
		if (_currentItem < GetItemCount() - 1) {
			return *(wxString*)GetItemData(++_currentItem);
		}
		else {
			return "";
		}
	}

	wxString Playlist::GetPreviousItem() {
		if (_currentItem > 0L) {
			return *(wxString*)GetItemData(--_currentItem);
		}
		else {
			return "";
		}
	}
}