/*
 *   Copyright (C) 2009,2013 by Jonathan Naylor G4KLX
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

#include "DVTOOLFileReader.h"
#include "DStarDefines.h"
#include "Utils.h"

#include <wx/wx.h>

static const char              DVTOOL_SIGNATURE[] = "DVTOOL";
static const unsigned int DVTOOL_SIGNATURE_LENGTH = 6U;

static const char              DSVT_SIGNATURE[] = "DSVT";
static const unsigned int DSVT_SIGNATURE_LENGTH = 4U;

static const unsigned int FIXED_DATA_LENGTH = 9U;

static const unsigned char HEADER_FLAG   = 0x10;
static const unsigned char DATA_FLAG     = 0x20;

static const unsigned char HEADER_MASK   = 0x80;
static const unsigned char TRAILER_MASK  = 0x40;

const unsigned int BUFFER_LENGTH = 255U;


CDVTOOLFileReader::CDVTOOLFileReader() :
m_fileName(),
m_file(),
m_records(0U),
m_type(DVTFR_NONE),
m_buffer(NULL),
m_length(0U),
m_end(false)
{
	m_buffer = new unsigned char[BUFFER_LENGTH];
}

CDVTOOLFileReader::~CDVTOOLFileReader()
{
	delete[] m_buffer;
}

wxString CDVTOOLFileReader::getFileName() const
{
	return m_fileName;
}

unsigned int CDVTOOLFileReader::getRecords() const
{
	return m_records;
}

bool CDVTOOLFileReader::open(const wxString& fileName)
{
	m_fileName = fileName;

	bool res = m_file.Open(fileName, wxT("rb"));
	if (!res)
		return false;

	unsigned char buffer[DVTOOL_SIGNATURE_LENGTH];
	size_t n = m_file.Read(buffer, DVTOOL_SIGNATURE_LENGTH);
	if (n != DVTOOL_SIGNATURE_LENGTH) {
		m_file.Close();
		return false;
	}

	if (::memcmp(buffer, DVTOOL_SIGNATURE, DVTOOL_SIGNATURE_LENGTH) != 0) {
		m_file.Close();
		return false;
	}

	wxUint32 uint32;
	n = m_file.Read(&uint32, sizeof(wxUint32));
	if (n != sizeof(wxUint32)) {
		m_file.Close();
		return false;
	}

	m_records = wxUINT32_SWAP_ON_LE(uint32);
	m_end     = false;

	return true;
}

DVTFR_TYPE CDVTOOLFileReader::read()
{
	wxUint16 uint16;
	size_t n = m_file.Read(&uint16, sizeof(wxUint16));
	if (n != sizeof(wxUint16))
		return DVTFR_NONE;

	m_length = wxUINT16_SWAP_ON_BE(uint16) - 15U;

	unsigned char bytes[FIXED_DATA_LENGTH];
	n = m_file.Read(bytes, DSVT_SIGNATURE_LENGTH);
	if (n != DSVT_SIGNATURE_LENGTH)
		return DVTFR_NONE;

	if (::memcmp(bytes, DSVT_SIGNATURE, DSVT_SIGNATURE_LENGTH) != 0)
		return DVTFR_NONE;

	char flag;
	n = m_file.Read(&flag, 1U);
	if (n != 1U)
		return DVTFR_NONE;

	m_type = (flag == HEADER_FLAG) ? DVTFR_HEADER : DVTFR_DATA;

	n = m_file.Read(bytes, FIXED_DATA_LENGTH);
	if (n != FIXED_DATA_LENGTH)
		return DVTFR_NONE;

	n = m_file.Read(&flag, 1U);
	if (n != 1U)
		return DVTFR_NONE;

	if (m_type == DVTFR_DATA) {
		if ((flag & TRAILER_MASK) == TRAILER_MASK)
			m_end = true;
	}

	n = m_file.Read(m_buffer, m_length);
	if (n != m_length)
		return DVTFR_NONE;

	return m_type;
}

CHeaderData* CDVTOOLFileReader::readHeader()
{
	if (m_type != DVTFR_HEADER)
		return NULL;

	if (m_buffer[39U] == 0xFFU && m_buffer[40U] == 0xFFU)
		return new CHeaderData(m_buffer, RADIO_HEADER_LENGTH_BYTES, false);

	// Header checksum testing is enabled
	CHeaderData* header = new CHeaderData(m_buffer, RADIO_HEADER_LENGTH_BYTES, true);

	if (!header->isValid()) {
		CUtils::dump(wxT("Header checksum failure"), m_buffer, RADIO_HEADER_LENGTH_BYTES);
		delete header;
		return NULL;
	}

	return header;
}

unsigned int CDVTOOLFileReader::readData(unsigned char* buffer, unsigned int length, bool& end)
{
	wxASSERT(buffer != NULL);
	wxASSERT(length > 0U);

	if (m_type != DVTFR_DATA)
		return 0U;

	if (length > m_length)
		length = m_length;

	end = m_end;

	::memcpy(buffer, m_buffer, length);

	return length;
}

void CDVTOOLFileReader::close()
{
	m_file.Close();
}
