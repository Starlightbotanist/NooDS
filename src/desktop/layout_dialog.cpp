/*
    Copyright 2020 Hydr8gon

    This file is part of NooDS.

    NooDS is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NooDS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NooDS. If not, see <https://www.gnu.org/licenses/>.
*/

#include "layout_dialog.h"
#include "noo_app.h"

enum Event
{
    ROTATE_NONE = 1,
    ROTATE_CW,
    ROTATE_CCW,
    ARRANGE_AUTO,
    ARRANGE_VERT,
    ARRANGE_HORI,
    SIZE_EVEN,
    SIZE_TOP,
    SIZE_BOT,
    GAP,
    INT_SCALE,
    FILTER
};

wxBEGIN_EVENT_TABLE(LayoutDialog, wxDialog)
EVT_RADIOBUTTON(ROTATE_NONE,  LayoutDialog::rotateNone)
EVT_RADIOBUTTON(ROTATE_CW,    LayoutDialog::rotateCw)
EVT_RADIOBUTTON(ROTATE_CCW,   LayoutDialog::rotateCcw)
EVT_RADIOBUTTON(ARRANGE_AUTO, LayoutDialog::arrangeAuto)
EVT_RADIOBUTTON(ARRANGE_VERT, LayoutDialog::arrangeVert)
EVT_RADIOBUTTON(ARRANGE_HORI, LayoutDialog::arrangeHori)
EVT_RADIOBUTTON(SIZE_EVEN,    LayoutDialog::sizeEven)
EVT_RADIOBUTTON(SIZE_TOP,     LayoutDialog::sizeTop)
EVT_RADIOBUTTON(SIZE_BOT,     LayoutDialog::sizeBot)
EVT_CHECKBOX(GAP,             LayoutDialog::gap)
EVT_CHECKBOX(INT_SCALE,       LayoutDialog::intScale)
EVT_CHECKBOX(FILTER,          LayoutDialog::filter)
EVT_BUTTON(wxID_CANCEL,       LayoutDialog::cancel)
wxEND_EVENT_TABLE()

LayoutDialog::LayoutDialog(NooFrame *frame): wxDialog(nullptr, wxID_ANY, "Screen Layout"), frame(frame)
{
    // Remember the previous settings in case the changes are discarded
    prevSettings[0] = NooApp::getScreenRotation();
    prevSettings[1] = NooApp::getScreenArrangement();
    prevSettings[2] = NooApp::getScreenSizing();
    prevSettings[3] = NooApp::getScreenGap();
    prevSettings[4] = NooApp::getScreenFilter();
    prevSettings[5] = NooApp::getIntegerScale();

    // Determine the height of a button
    // Borders are measured in pixels, so this value can be used to make values that scale with the DPI/font size
    wxButton *dummy = new wxButton(this, wxID_ANY, "");
    int size = dummy->GetSize().y;
    delete dummy;

    // Set up the rotation settings
    wxRadioButton *rotateBtns[3];
    wxBoxSizer *rotateSizer = new wxBoxSizer(wxHORIZONTAL);
    rotateSizer->Add(new wxStaticText(this, wxID_ANY, "Rotation:", wxDefaultPosition,
        wxSize(wxDefaultSize.GetWidth(), size)), 1, wxALIGN_CENTRE | wxRIGHT, size / 8);
    rotateSizer->Add(rotateBtns[0] = new wxRadioButton(this, ROTATE_NONE, "None",
        wxDefaultPosition, wxDefaultSize, wxRB_GROUP), 0, wxLEFT, size / 8);
    rotateSizer->Add(rotateBtns[1] = new wxRadioButton(this, ROTATE_CW,  "Clockwise"),         0, wxLEFT, size / 8);
    rotateSizer->Add(rotateBtns[2] = new wxRadioButton(this, ROTATE_CCW, "Counter-Clockwise"), 0, wxLEFT, size / 8);

    // Set up the arrangement settings
    wxRadioButton *arrangeBtns[3];
    wxBoxSizer *arrangeSizer = new wxBoxSizer(wxHORIZONTAL);
    arrangeSizer->Add(new wxStaticText(this, wxID_ANY, "Arrangement:", wxDefaultPosition,
        wxSize(wxDefaultSize.GetWidth(), size)), 0, wxALIGN_CENTRE | wxRIGHT, size / 8);
    arrangeSizer->Add(arrangeBtns[0] = new wxRadioButton(this, ARRANGE_AUTO, "Automatic",
        wxDefaultPosition, wxDefaultSize, wxRB_GROUP), 0, wxLEFT, size / 8);
    arrangeSizer->Add(arrangeBtns[1] = new wxRadioButton(this, ARRANGE_VERT, "Vertical"),   0, wxLEFT, size / 8);
    arrangeSizer->Add(arrangeBtns[2] = new wxRadioButton(this, ARRANGE_HORI, "Horizontal"), 0, wxLEFT, size / 8);

    // Set up the sizing settings
    wxRadioButton *sizeBtns[3];
    wxBoxSizer *sizeSizer = new wxBoxSizer(wxHORIZONTAL);
    sizeSizer->Add(new wxStaticText(this, wxID_ANY, "Sizing:", wxDefaultPosition,
        wxSize(wxDefaultSize.GetWidth(), size)), 0, wxALIGN_CENTRE | wxRIGHT, size / 8);
    sizeSizer->Add(sizeBtns[0] = new wxRadioButton(this, SIZE_EVEN, "Even",
        wxDefaultPosition, wxDefaultSize, wxRB_GROUP), 0, wxLEFT, size / 8);
    sizeSizer->Add(sizeBtns[1] = new wxRadioButton(this, SIZE_TOP, "Enlarge Top"),    0, wxLEFT, size / 8);
    sizeSizer->Add(sizeBtns[2] = new wxRadioButton(this, SIZE_BOT, "Enlarge Bottom"), 0, wxLEFT, size / 8);

    // Set up the checkbox settings
    wxCheckBox *boxes[3];
    wxBoxSizer *checkSizer = new wxBoxSizer(wxHORIZONTAL);
    checkSizer->Add(boxes[0] = new wxCheckBox(this, GAP,       "Gap"));
    checkSizer->Add(boxes[1] = new wxCheckBox(this, FILTER,    "Filter"),        0, wxLEFT, size / 8);
    checkSizer->Add(boxes[2] = new wxCheckBox(this, INT_SCALE, "Integer Scale"), 0, wxLEFT, size / 8);

    // Set the current values of the radio buttons
    if (NooApp::getScreenRotation() < 3)
        rotateBtns[NooApp::getScreenRotation()]->SetValue(true);
    if (NooApp::getScreenArrangement() < 3)
        arrangeBtns[NooApp::getScreenArrangement()]->SetValue(true);
    if (NooApp::getScreenSizing() < 3)
        sizeBtns[NooApp::getScreenSizing()]->SetValue(true);

    // Set the current values of the checkboxes
    boxes[0]->SetValue(NooApp::getScreenGap());
    boxes[1]->SetValue(NooApp::getScreenFilter());
    boxes[2]->SetValue(NooApp::getIntegerScale());

    // Set up the cancel and confirm buttons
    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxStaticText(this, wxID_ANY, ""), 1);
    buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"),  0, wxRIGHT, size / 16);
    buttonSizer->Add(new wxButton(this, wxID_OK,     "Confirm"), 0, wxLEFT,  size / 16);

    // Combine all of the contents
    wxBoxSizer *contents = new wxBoxSizer(wxVERTICAL);
    contents->Add(rotateSizer,  1, wxEXPAND);
    contents->Add(arrangeSizer, 1, wxEXPAND);
    contents->Add(sizeSizer,    1, wxEXPAND);
    contents->Add(checkSizer,   1, wxEXPAND);
    contents->Add(buttonSizer,  1, wxEXPAND);

    // Add a final border around everything
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(contents, 1, wxEXPAND | wxALL, size / 4);
    SetSizer(sizer);

    // Size the window to fit the contents and prevent resizing
    sizer->Fit(this);
    SetMinSize(GetSize());
    SetMaxSize(GetSize());
}

void LayoutDialog::rotateNone(wxCommandEvent &event)
{
    // Set the screen rotation setting to none
    NooApp::setScreenRotation(0);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::rotateCw(wxCommandEvent &event)
{
    // Set the screen rotation setting to clockwise
    NooApp::setScreenRotation(1);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::rotateCcw(wxCommandEvent &event)
{
    // Set the screen rotation setting to counter-clockwise
    NooApp::setScreenRotation(2);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::arrangeAuto(wxCommandEvent &event)
{
    // Set the screen arrangement setting to automatic
    NooApp::setScreenArrangement(0);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::arrangeVert(wxCommandEvent &event)
{
    // Set the screen arrangement setting to vertical
    NooApp::setScreenArrangement(1);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::arrangeHori(wxCommandEvent &event)
{
    // Set the screen arrangement setting to horizontal
    NooApp::setScreenArrangement(2);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::sizeEven(wxCommandEvent &event)
{
    // Set the screen sizing setting to even
    NooApp::setScreenSizing(0);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::sizeTop(wxCommandEvent &event)
{
    // Set the screen sizing setting to enlarge top
    NooApp::setScreenSizing(1);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::sizeBot(wxCommandEvent &event)
{
    // Set the screen sizing setting to enlarge bottom
    NooApp::setScreenSizing(2);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::gap(wxCommandEvent &event)
{
    // Toggle the screen gap setting
    NooApp::setScreenGap(!NooApp::getScreenGap());

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::filter(wxCommandEvent &event)
{
    // Toggle the screen filter setting
    NooApp::setScreenFilter(!NooApp::getScreenFilter());

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::intScale(wxCommandEvent &event)
{
    // Toggle the integer scale setting
    NooApp::setIntegerScale(!NooApp::getIntegerScale());

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();
}

void LayoutDialog::cancel(wxCommandEvent &event)
{
    // Reset the settings to their previous values
    NooApp::setScreenRotation(prevSettings[0]);
    NooApp::setScreenArrangement(prevSettings[1]);
    NooApp::setScreenSizing(prevSettings[2]);
    NooApp::setScreenGap(prevSettings[3]);
    NooApp::setScreenFilter(prevSettings[4]);
    NooApp::setIntegerScale(prevSettings[5]);

    // Trigger a resize to update the screen layout
    frame->SendSizeEvent();

    event.Skip(true);
}
