#include "MtrkHeader.h"
#include "MetaEventText.h"
#include "boost/log/trivial.hpp"

MtrkHeader::MtrkHeader()
{

}

MtrkHeader::MtrkHeader(ByteStream &stream, const uint16_t bpm, const uint16_t ppqn)
	: m_bpm(bpm), m_ppqn(ppqn)
{
	m_mtrk = stream.get_text_header();
	m_size = stream.get32u_bswap();
	m_startPos = stream.tellg();
	m_trackPreDelay = VLV(stream);

	while(stream.tellg() - m_startPos < m_size)
	{
		m_events.push_back(Event(stream));
	}
	m_endPos = stream.tellg();
	BOOST_LOG_TRIVIAL(info) << "MTrk header total read bytes " << m_endPos - m_startPos
	<< " == " << m_size << " header size";
	BOOST_LOG_TRIVIAL(info) << "MTrk header pos at end: " << m_endPos;
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
	BOOST_LOG_TRIVIAL(info) << "MTrk pulses per second: " << getPulsesPerSecond();
	BOOST_LOG_TRIVIAL(info) << "MTrk predelay value: " << m_trackPreDelay.getValue();
	BOOST_LOG_TRIVIAL(info) << "MTrk count of events: " << m_events.size();
	for(auto event : m_events)
		event.log();
}

bool MtrkHeader::isOk()
{
	return g_mtrk_reference == m_mtrk;
}

std::vector<Event> MtrkHeader::getEvents()
{
	return m_events;
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

double MtrkHeader::getPulsesPerSecond()
{
	return (double)(60000.0 / ( m_bpm * m_ppqn));
}

double MtrkHeader::getPreDelayMs()
{
	return ((double)m_trackPreDelay.getValue() * getPulsesPerSecond());
}

void MtrkHeader::updateBpm(const uint16_t bpm)
{
	m_bpm = bpm;
}

uint16_t MtrkHeader::getBpm()
{
	return m_bpm;
}

std::string MtrkHeader::getName()
{
	for(auto event : m_events)
	{
		if(event.getCmd().getFullCmd() == TRACK_NAME)
			return MetaEventText(event).getText();
	}
	return std::string("<UNKNOWN>");
}

std::string MtrkHeader::getInstrumentName()
{
	for(auto event : m_events)
	{
		if(event.getCmd().getFullCmd() == INSTRUMENT_NAME)
			return MetaEventText(event).getText();
	}
	return std::string("<UNKNOWN>");
}
