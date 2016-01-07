/*
 *   Copyright (C) 2011-2015 by Jonathan Naylor G4KLX
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

#include "DStarRepeaterConfigDVMegaSet.h"
#include "SerialPortSelector.h"

const unsigned int BORDER_SIZE    = 5U;
const unsigned int CONTROL_WIDTH1 = 150U;
const unsigned int CONTROL_WIDTH2 = 300U;

const unsigned int PORT_LENGTH     = 5U;

const int CHOICE_VARIANT = 8770;

BEGIN_EVENT_TABLE(CDStarRepeaterConfigDVMegaSet, wxDialog)
	EVT_CHOICE(CHOICE_VARIANT, CDStarRepeaterConfigDVMegaSet::onVariant)
END_EVENT_TABLE()


CDStarRepeaterConfigDVMegaSet::CDStarRepeaterConfigDVMegaSet(wxWindow* parent, int id, const wxString& port, DVMEGA_VARIANT variant, bool rxInvert, bool txInvert, unsigned int txDelay, unsigned int rxFrequency, unsigned int txFrequency, unsigned int power) :
wxDialog(parent, id, wxString(_("DVMEGA Settings"))),
m_port(NULL),
m_variant(NULL),
m_txInvert(NULL),
m_rxInvert(NULL),
m_txDelay(NULL),
m_frequency(NULL),
m_power(NULL)
{
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);

	wxStaticText* portLabel = new wxStaticText(this, -1, _("Port"));
	sizer->Add(portLabel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	m_port = new wxChoice(this, -1, wxDefaultPosition, wxSize(CONTROL_WIDTH1, -1));
	sizer->Add(m_port, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	wxArrayString ports = CSerialPortSelector::getDevices();
	for (unsigned int i = 0U; i < ports.GetCount(); i++)
		m_port->Append(ports.Item(i));

	bool found = m_port->SetStringSelection(port);
	if (!found)
		m_port->SetSelection(0);

	wxStaticText* variantLabel = new wxStaticText(this, -1, _("Variant"));
	sizer->Add(variantLabel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	m_variant = new wxChoice(this, CHOICE_VARIANT, wxDefaultPosition, wxSize(CONTROL_WIDTH1, -1));
	m_variant->Append(_("Modem"));
	m_variant->Append(_("Radio - 2m"));
	m_variant->Append(_("Radio - 70cm"));
	m_variant->Append(_("Radio - 2m + 70cm"));
	sizer->Add(m_variant, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);
	m_variant->SetSelection(int(variant));

	wxStaticText* txInvertLabel = new wxStaticText(this, -1, _("TX Inversion"));
	sizer->Add(txInvertLabel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	m_txInvert = new wxChoice(this, -1, wxDefaultPosition, wxSize(CONTROL_WIDTH1, -1));
	m_txInvert->Append(_("Off"));
	m_txInvert->Append(_("On"));
	sizer->Add(m_txInvert, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);
	m_txInvert->SetSelection(txInvert ? 1 : 0);

	wxStaticText* rxInvertLabel = new wxStaticText(this, -1, _("RX Inversion"));
	sizer->Add(rxInvertLabel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	m_rxInvert = new wxChoice(this, -1, wxDefaultPosition, wxSize(CONTROL_WIDTH1, -1));
	m_rxInvert->Append(_("Off"));
	m_rxInvert->Append(_("On"));
	sizer->Add(m_rxInvert, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);
	m_rxInvert->SetSelection(rxInvert ? 1 : 0);

	wxStaticText* freqLabel = new wxStaticText(this, -1, _("Frequency (Hz)"));
	sizer->Add(freqLabel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	wxString text;
	if (rxFrequency == txFrequency)
	    text.Printf(wxT("%u"), rxFrequency);
        else
            text.Printf(wxT("%u/%u"), rxFrequency, txFrequency);

        m_frequency = new wxTextCtrl(this, -1, text, wxDefaultPosition, wxSize(CONTROL_WIDTH1, -1));
	sizer->Add(m_frequency, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	wxStaticText* txDelayLabel = new wxStaticText(this, -1, _("TX Delay (ms)"));
	sizer->Add(txDelayLabel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	m_txDelay = new wxSlider(this, -1, txDelay, 0, 350, wxDefaultPosition, wxSize(CONTROL_WIDTH2, -1), wxSL_HORIZONTAL | wxSL_LABELS);
	sizer->Add(m_txDelay, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	wxStaticText* powerLabel = new wxStaticText(this, -1, _("Power (%)"));
	sizer->Add(powerLabel, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	m_power = new wxSlider(this, -1, power, 1, 100, wxDefaultPosition, wxSize(CONTROL_WIDTH2, -1), wxSL_HORIZONTAL | wxSL_LABELS);
	sizer->Add(m_power, 0, wxALL | wxALIGN_LEFT, BORDER_SIZE);

	topSizer->Add(sizer);

	topSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, BORDER_SIZE);

	switch (variant) {
		case DVMV_RADIO_2M:
		case DVMV_RADIO_70CM:
		case DVMV_RADIO_2M_70CM:
			m_txInvert->Disable();
			m_rxInvert->Disable();
			break;
		default:
			m_frequency->Disable();
			m_power->Disable();
			break;
	}

	SetAutoLayout(true);

	topSizer->Fit(this);
	topSizer->SetSizeHints(this);

	SetSizer(topSizer);
}

CDStarRepeaterConfigDVMegaSet::~CDStarRepeaterConfigDVMegaSet()
{
}

bool CDStarRepeaterConfigDVMegaSet::Validate()
{
	if (m_port->GetCurrentSelection() == wxNOT_FOUND)
		return false;

	if (m_variant->GetCurrentSelection() == wxNOT_FOUND)
		return false;

	if (m_txInvert->GetCurrentSelection() == wxNOT_FOUND)
		return false;

	if (m_rxInvert->GetCurrentSelection() == wxNOT_FOUND)
		return false;

	if (m_variant->GetSelection() == 1) {
		unsigned int frequency = getRXFrequency();

                if (frequency >= 144000000U && frequency < 148000000U)
                        return true;
                else
                        return false;
	} else if (m_variant->GetSelection() == 2) {
		unsigned int frequency = getRXFrequency();

                if (frequency >= 420000000U && frequency < 450000000U)
                        return true;
                else
                        return false;
	} else if (m_variant->GetSelection() == 3) {
		unsigned int rxFrequency = getRXFrequency();
		unsigned int txFrequency = getTXFrequency();

		if (rxFrequency == txFrequency) {
                        if ((rxFrequency >= 144000000U && rxFrequency < 148000000U) ||
		            (rxFrequency >= 420000000U && rxFrequency < 450000000U))
                                return true;
                        else
                                return false;
                } else {
        		bool rx2m   = (rxFrequency >= 144000000U && rxFrequency < 148000000U);
			bool rx70cm = (rxFrequency >= 420000000U && rxFrequency < 450000000U);

			bool tx2m   = (txFrequency >= 144000000U && txFrequency < 148000000U);
        		bool tx70cm = (txFrequency >= 420000000U && txFrequency < 450000000U);

        		if (rx2m && tx70cm)
        		    return true;
                        else if (rx70cm && tx2m)
                            return true;
                        else
                            return false;
                }
	}

	return true;
}

wxString CDStarRepeaterConfigDVMegaSet::getPort() const
{
	int n = m_port->GetCurrentSelection();

	if (n == wxNOT_FOUND)
		return wxEmptyString;

	return m_port->GetStringSelection();
}

DVMEGA_VARIANT CDStarRepeaterConfigDVMegaSet::getVariant() const
{
	int n = m_variant->GetCurrentSelection();

	if (n == wxNOT_FOUND)
		return DVMV_MODEM;

	return DVMEGA_VARIANT(n);
}

bool CDStarRepeaterConfigDVMegaSet::getRXInvert() const
{
	int n = m_rxInvert->GetCurrentSelection();

	if (n == wxNOT_FOUND)
		return false;

	return n == 1;
}

bool CDStarRepeaterConfigDVMegaSet::getTXInvert() const
{
	int n = m_txInvert->GetCurrentSelection();

	if (n == wxNOT_FOUND)
		return false;

	return n == 1;
}

unsigned int CDStarRepeaterConfigDVMegaSet::getTXDelay() const
{
	return (unsigned int)m_txDelay->GetValue();
}

unsigned int CDStarRepeaterConfigDVMegaSet::getRXFrequency() const
{
	wxString hz = m_frequency->GetValue();

	int pos = hz.Find(wxT('/'));
	if (pos != wxNOT_FOUND) {
            unsigned long frequency;
            hz.Left(pos).ToULong(&frequency);
            return frequency;
        } else {
	    unsigned long frequency;
	    hz.ToULong(&frequency);
	    return frequency;
        }
}

unsigned int CDStarRepeaterConfigDVMegaSet::getTXFrequency() const
{
        DVMEGA_VARIANT variant = getVariant();
	wxString hz = m_frequency->GetValue();

	int pos = hz.Find(wxT('/'));
	if (pos != wxNOT_FOUND && variant == DVMV_RADIO_2M_70CM) {
            unsigned long frequency;
            hz.Mid(pos + 1).ToULong(&frequency);
            return frequency;
        } else {
            if (pos != wxNOT_FOUND) {
                unsigned long frequency;
                hz.Left(pos).ToULong(&frequency);
                return frequency;
            } else {
	        unsigned long frequency;
	        hz.ToULong(&frequency);
	        return frequency;
            }
        }
}

unsigned int CDStarRepeaterConfigDVMegaSet::getPower() const
{
	return (unsigned int)m_power->GetValue();
}

void CDStarRepeaterConfigDVMegaSet::onVariant(wxCommandEvent &event)
{
	DVMEGA_VARIANT variant = getVariant();

	switch (variant) {
		case DVMV_RADIO_2M:
		case DVMV_RADIO_70CM:
		case DVMV_RADIO_2M_70CM:
			m_txInvert->Disable();
			m_rxInvert->Disable();
			m_frequency->Enable();
			m_power->Enable();
			break;
		default:	// DVMV_MODEM
			m_txInvert->Enable();
			m_rxInvert->Enable();
			m_frequency->Disable();
			m_power->Disable();
			break;
	}
}
