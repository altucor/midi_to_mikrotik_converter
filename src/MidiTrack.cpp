#include "MidiTrack.hpp"
#include "MetaEventText.hpp"
#include "boost/log/trivial.hpp"

const static std::string unknownTxtValue = "<UNKNOWN>";

MidiTrack::MidiTrack()
{

}

MidiTrack::MidiTrack(ByteStream &stream, const uint16_t bpm, const uint16_t ppqn)
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


	m_trackName = unknownTxtValue;
	m_instrumentName = unknownTxtValue;
	for(auto event : m_events)
	{
		if(event.getCmd().getFullCmd() == TRACK_NAME)
			m_trackName = MetaEventText(event).getText();
		if(event.getCmd().getFullCmd() == INSTRUMENT_NAME)
			m_instrumentName = MetaEventText(event).getText();
	}

	BOOST_LOG_TRIVIAL(info) << "MTrk header total read bytes " << m_endPos - m_startPos
	<< " == " << m_size << " header size";
	BOOST_LOG_TRIVIAL(info) << "MTrk header pos at end: " << m_endPos;
}

MidiTrack::~MidiTrack()
{
}

void MidiTrack::log()
{
	BOOST_LOG_TRIVIAL(info) << "MTrk header: " << m_mtrk;
	BOOST_LOG_TRIVIAL(info) << "MTrk start  pos: " << m_startPos;
	BOOST_LOG_TRIVIAL(info) << "MTrk end    pos: " << m_endPos;
	BOOST_LOG_TRIVIAL(info) << "MTrk chunk size: " << m_size;
	BOOST_LOG_TRIVIAL(info) << "MTrk pulses per second: " << getPulsesPerSecond();
	BOOST_LOG_TRIVIAL(info) << "MTrk predelay value: " << m_trackPreDelay.getValue();
	BOOST_LOG_TRIVIAL(info) << "MTrk count of events: " << m_events.size();
	//for(auto event : m_events)
	//	event.log();
}

bool MidiTrack::isOk()
{
	return g_mtrk_reference == m_mtrk;
}

std::vector<Event> MidiTrack::getEvents()
{
	return m_events;
}

double MidiTrack::getPulsesPerSecond()
{
	return (double)(60000.0 / ( m_bpm * m_ppqn));
}

double MidiTrack::getPreDelayMs()
{
	return ((double)m_trackPreDelay.getValue() * getPulsesPerSecond());
}

void MidiTrack::updateBpm(const uint16_t bpm)
{
	m_bpm = bpm;
}

uint16_t MidiTrack::getBpm()
{
	return m_bpm;
}

std::string MidiTrack::getName()
{
	return m_trackName;
}

std::string MidiTrack::getInstrumentName()
{
	return m_instrumentName;
}
