#include "MtrkHeader.h"
#include "boost/log/trivial.hpp"

MtrkHeader::MtrkHeader(ByteStream &stream, double pulsesPerSec)
{
	m_pulsesPerSec = pulsesPerSec;
	m_mtrk = stream.get_text_header();
	m_size = stream.get32u_bswap();
	m_startPos = stream.tellg();
	//m_track_predelay = VLV(stream);
	uint8_t cmd = 0;
	bool vlvExpected = true;
	VLV vlv;
	while(stream.tellg() - m_startPos < m_size)
	{
		if(vlvExpected)
			vlv = VLV(stream);
		vlvExpected = false;
		cmd = stream.get8u();
		if(cmd >= CHANNEL_1_NOTE_OFF && cmd <= CHANNEL_16_NOTE_ON)
		{
			m_midiNotes.push_back(Note(cmd, stream, m_pulsesPerSec));
			vlvExpected = false;
		}
		else
		{
			m_midiEvents.push_back(MidiEvent(cmd, stream, m_pulsesPerSec));
			vlvExpected = true;
		}
	}
	BOOST_LOG_TRIVIAL(info) << "MTrk header total read bytes: " << stream.tellg() - m_startPos;
	BOOST_LOG_TRIVIAL(info) << "MTrk header pos at end: " << stream.tellg();
}

MtrkHeader::~MtrkHeader()
{
}

void MtrkHeader::log()
{
	BOOST_LOG_TRIVIAL(info) << "MTrk header: " << m_mtrk;
	BOOST_LOG_TRIVIAL(info) << "MTrk start pos: " << m_startPos;
	BOOST_LOG_TRIVIAL(info) << "MTrk end   pos: " << m_endPos;
	BOOST_LOG_TRIVIAL(info) << "MTrk chunk size: " << m_size;
	BOOST_LOG_TRIVIAL(info) << "MTrk count of Midi events: " << m_midiEvents.size();
	for(auto event : m_midiEvents)
		event.log();
	for(auto note : m_midiNotes)
		note.log();
}

bool MtrkHeader::isOk()
{
	return g_mtrk_reference == m_mtrk;
}

bool MtrkHeader::isServiceTrack()
{
	// If there is only midi events in track
	// and there is no midi NOTE events than this track has
	// service type
	return m_midiEvents.size() != 0 && m_midiNotes.size() == 0;
}

uint64_t MtrkHeader::getStartPos()
{
	return m_startPos;
}

uint64_t MtrkHeader::getEndPos()
{
	return m_endPos;
}

uint64_t MtrkHeader::getSize()
{
	return m_size;
}

std::vector<MidiEvent> MtrkHeader::getEvents()
{
	return m_midiEvents;
}

std::vector<Note> MtrkHeader::getNotes()
{
	return m_midiNotes;
}
