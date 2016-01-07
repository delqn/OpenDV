/*
 *   Copyright (C) 2011,2012 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "RemoteControlRepeaterPanel.h"
#include "RemoteControlApp.h"
#include "DStarDefines.h"
#include "HostFile.h"

#include <wx/filename.h>

enum {
	Button_Refresh = 7100,
	Button_Link,
	Button_Unlink
};

BEGIN_EVENT_TABLE(CRemoteControlRepeaterPanel, wxPanel)
	EVT_BUTTON(Button_Refresh, CRemoteControlRepeaterPanel::onRefresh)
	EVT_BUTTON(Button_Link,    CRemoteControlRepeaterPanel::onLink)
	EVT_BUTTON(Button_Unlink,  CRemoteControlRepeaterPanel::onUnlink)
END_EVENT_TABLE()

#if defined(__WINDOWS__)
const unsigned int LIST_HEIGHT     = 350U;
const unsigned int LIST_WIDTH      = 350U;
const unsigned int CALLSIGN_WIDTH  = 100U;
const unsigned int PROTOCOL_WIDTH  = 55U;
const unsigned int DIRECTION_WIDTH = 55U;
const unsigned int TYPE_WIDTH      = 65U;
const unsigned int STATE_WIDTH     = 55U;
const unsigned int BUTTON_WIDTH    = 75U;
const unsigned int REFLECTOR_WIDTH = 75U;
const unsigned int CHANNEL_WIDTH   = 40U;
const unsigned int RECONNECT_WIDTH = 75U;
#else
const unsigned int LIST_HEIGHT     = 350U;
const unsigned int LIST_WIDTH      = 350U;
const unsigned int CALLSIGN_WIDTH  = 100U;
const unsigned int PROTOCOL_WIDTH  = 55U;
const unsigned int DIRECTION_WIDTH = 55U;
const unsigned int TYPE_WIDTH      = 65U;
const unsigned int STATE_WIDTH     = 55U;
const unsigned int BUTTON_WIDTH    = 75U;
const unsigned int REFLECTOR_WIDTH = 90U;
const unsigned int CHANNEL_WIDTH   = 50U;
const unsigned int RECONNECT_WIDTH = 100U;
#endif

const unsigned int BORDER_SIZE = 5U;

static int wxCALLBACK itemCompare(long item1, long item2, long sortData)
{
	PROTOCOL proto1 = PROTOCOL(item1);
	PROTOCOL proto2 = PROTOCOL(item2);

	if (proto1 == proto2)
		return 0;

	// DCS at the top
	if (proto1 == PROTO_DCS)
		return 1;

	if (proto2 == PROTO_DCS)
		return -1;

	// D-Plus comes next
	if (proto1 == PROTO_DPLUS)
		return 1;

	return -1;
}

CRemoteControlRepeaterPanel::CRemoteControlRepeaterPanel(wxWindow* parent, int id, const wxString& callsign) :
wxPanel(parent, id),
m_callsign(callsign),
m_list(NULL),
m_reflector(NULL),
m_channel(NULL),
m_reconnect(NULL)
{
	wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);

	m_list = new wxListCtrl(this, -1, wxDefaultPosition, wxSize(LIST_WIDTH, LIST_HEIGHT), wxLC_REPORT | wxLC_SINGLE_SEL);
	m_list->InsertColumn(0L, _("Callsign"));
	m_list->SetColumnWidth(0L, CALLSIGN_WIDTH);
	m_list->InsertColumn(1L, _("Protocol"));
	m_list->SetColumnWidth(1L, PROTOCOL_WIDTH);
	m_list->InsertColumn(2L, _("Direction"));
	m_list->SetColumnWidth(2L, DIRECTION_WIDTH);
	m_list->InsertColumn(3L, _("Type"));
	m_list->SetColumnWidth(3L, TYPE_WIDTH);
	m_list->InsertColumn(4L, _("State"));
	m_list->SetColumnWidth(4L, STATE_WIDTH);
	sizer1->Add(m_list, 0, wxTOP | wxBOTTOM | wxLEFT | wxEXPAND, BORDER_SIZE);

	wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);

	wxButton* refreshButton = new wxButton(this, Button_Refresh, _("Refresh"), wxDefaultPosition, wxSize(BUTTON_WIDTH, -1));
	sizer2->Add(refreshButton, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy1Label = new wxStaticText(this, -1, wxEmptyString);
	sizer2->Add(dummy1Label, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy2Label = new wxStaticText(this, -1, wxEmptyString);
	sizer2->Add(dummy2Label, 0, wxALL, BORDER_SIZE);

	wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);

	m_reflector = new wxChoice(this, -1, wxDefaultPosition, wxSize(REFLECTOR_WIDTH, -1));
	m_reflector->Append(_("None"));

	wxFileName fileName1(wxFileName::GetHomeDir(), DPLUS_HOSTS_FILE_NAME);
	if (!fileName1.IsFileReadable()) {
#if defined(__WINDOWS__)
		fileName1.Assign(::wxGetCwd(), DPLUS_HOSTS_FILE_NAME);
#else
		fileName1.Assign(wxT(DATA_DIR), DPLUS_HOSTS_FILE_NAME);
#endif
	}

	CHostFile file1(fileName1.GetFullPath(), false);

	for (unsigned int i = 0U; i < file1.getCount(); i++)
		m_reflector->Append(file1.getName(i));

	wxFileName fileName2(wxFileName::GetHomeDir(), DEXTRA_HOSTS_FILE_NAME);
	if (!fileName2.IsFileReadable()) {
#if defined(__WINDOWS__)
		fileName2.Assign(::wxGetCwd(), DEXTRA_HOSTS_FILE_NAME);
#else
		fileName2.Assign(wxT(DATA_DIR), DEXTRA_HOSTS_FILE_NAME);
#endif
	}

	CHostFile file2(fileName2.GetFullPath(), false);

	for (unsigned int i = 0U; i < file2.getCount(); i++)
		m_reflector->Append(file2.getName(i));

	wxFileName fileName3(wxFileName::GetHomeDir(), DCS_HOSTS_FILE_NAME);
	if (!fileName3.IsFileReadable()) {
#if defined(__WINDOWS__)
		fileName3.Assign(::wxGetCwd(), DCS_HOSTS_FILE_NAME);
#else
		fileName3.Assign(wxT(DATA_DIR), DCS_HOSTS_FILE_NAME);
#endif
	}

	CHostFile file3(fileName3.GetFullPath(), false);

	for (unsigned int i = 0U; i < file3.getCount(); i++)
		m_reflector->Append(file3.getName(i));

	sizer3->Add(m_reflector, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);
	m_reflector->SetSelection(0);

	m_channel = new wxChoice(this, -1, wxDefaultPosition, wxSize(CHANNEL_WIDTH, -1));
	m_channel->Append(wxT("A"));
	m_channel->Append(wxT("B"));
	m_channel->Append(wxT("C"));
	m_channel->Append(wxT("D"));
	m_channel->Append(wxT("E"));
	m_channel->Append(wxT("F"));
	m_channel->Append(wxT("G"));
	m_channel->Append(wxT("H"));
	m_channel->Append(wxT("I"));
	m_channel->Append(wxT("J"));
	m_channel->Append(wxT("K"));
	m_channel->Append(wxT("L"));
	m_channel->Append(wxT("M"));
	m_channel->Append(wxT("N"));
	m_channel->Append(wxT("O"));
	m_channel->Append(wxT("P"));
	m_channel->Append(wxT("Q"));
	m_channel->Append(wxT("R"));
	m_channel->Append(wxT("S"));
	m_channel->Append(wxT("T"));
	m_channel->Append(wxT("U"));
	m_channel->Append(wxT("V"));
	m_channel->Append(wxT("W"));
	m_channel->Append(wxT("X"));
	m_channel->Append(wxT("Y"));
	m_channel->Append(wxT("Z"));
	sizer3->Add(m_channel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);
	m_channel->SetSelection(0);

	sizer2->Add(sizer3);

	m_reconnect = new wxChoice(this, -1, wxDefaultPosition, wxSize(RECONNECT_WIDTH, -1));
	m_reconnect->Append(_("Never"));
	m_reconnect->Append(_("Fixed"));
	m_reconnect->Append(_("5 minutes"));
	m_reconnect->Append(_("10 minutes"));
	m_reconnect->Append(_("15 minutes"));
	m_reconnect->Append(_("20 minutes"));
	m_reconnect->Append(_("25 minutes"));
	m_reconnect->Append(_("30 minutes"));
	m_reconnect->Append(_("60 minutes"));
	m_reconnect->Append(_("90 minutes"));
	m_reconnect->Append(_("120 minutes"));
	m_reconnect->Append(_("180 minutes"));
	sizer2->Add(m_reconnect, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);
	m_reconnect->SetSelection(0);

	wxStaticText* dummy3Label = new wxStaticText(this, -1, wxEmptyString);
	sizer2->Add(dummy3Label, 0, wxALL, BORDER_SIZE);

	wxButton* linkButton = new wxButton(this, Button_Link, _("Link"), wxDefaultPosition, wxSize(BUTTON_WIDTH, -1));
	sizer2->Add(linkButton, 0, wxALL, BORDER_SIZE);

	wxButton* unlinkButton = new wxButton(this, Button_Unlink, _("Unlink"), wxDefaultPosition, wxSize(BUTTON_WIDTH, -1));
	sizer2->Add(unlinkButton, 0, wxALL, BORDER_SIZE);

	sizer1->Add(sizer2);

	SetAutoLayout(true);

	SetSizer(sizer1);
}


CRemoteControlRepeaterPanel::~CRemoteControlRepeaterPanel()
{
}

void CRemoteControlRepeaterPanel::add(const CRemoteControlRepeaterData& data)
{
	m_list->DeleteAllItems();

	RECONNECT reconnect = data.getReconnect();
	m_reconnect->SetSelection(int(reconnect));

	wxString reflector = data.getReflector();
	reflector.Append(wxT("        "));
	reflector.Truncate(LONG_CALLSIGN_LENGTH);

	bool res = m_channel->SetStringSelection(reflector.Right(1));
	if (!res)
		m_channel->SetSelection(0);

	reflector.Truncate(LONG_CALLSIGN_LENGTH - 1U);
	reflector.Append(wxT(" "));

	res = m_reflector->SetStringSelection(reflector);
	if (!res)
		m_reflector->SetSelection(0);

	unsigned int count = data.getLinkCount();
	for (unsigned int i = 0U; i < count; i++) {
		CRemoteControlLinkData& link = data.getLink(i);

		wxString   callsign = link.getCallsign();
		PROTOCOL   protocol = link.getProtocol();
		bool       isLinked = link.isLinked();
		DIRECTION direction = link.getDirection();
		bool       isDongle = link.isDongle();

		m_list->InsertItem(0L, callsign);

		// To allow for sorting
		m_list->SetItemData(0L, long(protocol));

		switch (protocol) {
			case PROTO_DEXTRA:
				m_list->SetItem(0L, 1, wxT("DExtra"));
				break;
			case PROTO_DPLUS:
				m_list->SetItem(0L, 1, wxT("D-Plus"));
				break;
			case PROTO_DCS:
				m_list->SetItem(0L, 1, wxT("DCS"));
				break;
			default:
				m_list->SetItem(0L, 1, wxT("?????"));
				break;
		}

		switch (direction){
			case DIR_INCOMING:
				m_list->SetItem(0L, 2, _("IN"));
				break;
			case DIR_OUTGOING:
				m_list->SetItem(0L, 2, _("OUT"));
				break;
			default:
				m_list->SetItem(0L, 2, wxT("???"));
				break;
		}

		if (isDongle)
			m_list->SetItem(0L, 3, _("Dongle"));
		else
			m_list->SetItem(0L, 3, _("Repeater"));

		if (isLinked)
			m_list->SetItem(0L, 4, _("Linked"));
		else
			m_list->SetItem(0L, 4, _("Linking"));
	}

	m_list->SortItems(itemCompare, 0L);
}

void CRemoteControlRepeaterPanel::onRefresh(wxCommandEvent& event)
{
	::wxGetApp().repeaterRefresh(m_callsign);
}

void CRemoteControlRepeaterPanel::onLink(wxCommandEvent& event)
{
	int n = m_reconnect->GetCurrentSelection();
	if (n == wxNOT_FOUND)
		return;

	RECONNECT reconnect = RECONNECT(n);

	n = m_reflector->GetCurrentSelection();
	if (n == wxNOT_FOUND)
		return;

	wxString reflector = wxEmptyString;

	if (n > 0) {
		int n = m_channel->GetCurrentSelection();
		if (n == wxNOT_FOUND)
			return;

		reflector = m_reflector->GetStringSelection();
		if (reflector.IsEmpty())
			return;

		wxString channel = m_channel->GetStringSelection();
		if (channel.IsEmpty())
			return;

		reflector.Append(wxT("        "));
		reflector.Truncate(LONG_CALLSIGN_LENGTH - 1U);
		reflector.Append(channel);
	}

	::wxGetApp().link(m_callsign, reconnect, reflector);
}

void CRemoteControlRepeaterPanel::onUnlink(wxCommandEvent& event)
{
	int n = m_reconnect->GetCurrentSelection();
	if (n == wxNOT_FOUND)
		return;

	RECONNECT reconnect = RECONNECT(n);

	::wxGetApp().link(m_callsign, reconnect, wxEmptyString);
}
