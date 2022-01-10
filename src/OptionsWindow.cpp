#include "../include/OptionsWindow.h"
namespace blimp {

	bool Optionswindow:: read()
	{
		std::string Text;
		std::ifstream ReadFile("OptionsFile.txt");
		while (getline(ReadFile, Text));
		{
			if (Text == "1")
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		ReadFile.close();
	}
	void Optionswindow::write(bool input)
	{
		std::ofstream OptionsFile("OptionsFile.txt");
		OptionsFile << input;
		OptionsFile.close();
	}
	Optionswindow::Optionswindow() :wxFrame(nullptr, wxID_ANY, "Options")
	{
		wxBoxSizer* verticalFileBox = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* horizontalFileBox = new wxBoxSizer(wxHORIZONTAL);
		wxCheckBox* autoplayCheckbox = new wxCheckBox(this, autoplayID, "Autoplay", wxPoint(), wxDefaultSize, 1L, wxDefaultValidator, "autoplayOption");
		autoplayCheckbox->SetValue(read());
		Layout();
		horizontalFileBox->Add(autoplayCheckbox);
		verticalFileBox->Add(horizontalFileBox);
		Show();
		Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &Optionswindow::AutoplayCheckboxClicked, this, autoplayID);
	}

	void Optionswindow::AutoplayCheckboxClicked(wxCommandEvent& event)
	{
		wxCheckBox* checkbox = wxDynamicCast(FindWindow(autoplayID), wxCheckBox);
		write(checkbox->IsChecked());
	}
}