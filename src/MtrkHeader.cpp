#include "../include/MtrkHeader.h"
#include "boost/log/trivial.hpp"

MtrkHeader::MtrkHeader(ByteStream &stream)
{
	m_mtrk = stream.get_text_header();
	m_size = stream.get32u_bswap();
	// here get current stream position
	
	m_startPos = stream.tellg();
	while(stream.tellg() - m_startPos < m_size)
	{
		MidiEvent midi(stream);
		m_midiEvents.push_back(midi);
	}
	BOOST_LOG_TRIVIAL(info) << "MTrk header pos at end: " << stream.tellg() - m_startPos;
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
	for(uint64_t i=0; i<m_midiEvents.size(); i++)
		m_midiEvents[i].log();
}

bool MtrkHeader::isOk()
{
	return g_mtrk_reference == m_mtrk;
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
