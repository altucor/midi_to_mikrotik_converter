#include "MidiFile.h"
#include "ByteStream.h"
#include "MidiEvent.h"
#include "EventTempo.h"

#include <boost/log/trivial.hpp>
#include <fstream>

MidiFile::MidiFile(
	std::string &filePath, 
	const uint32_t bpm
)
	: m_filePath(filePath), m_bpm(bpm)
{

}

MidiFile::~MidiFile()
{
	
}

int MidiFile::processV0()
{
	return 0;
}

int MidiFile::processV1()
{
	if(m_mtrkTracks.size() == 0)
	{
		BOOST_LOG_TRIVIAL(error) << "No tracks found for processing";
		return -1;
	}

	MtrkHeader track = m_mtrkTracks[0];
	// Read BPM from MIDI File 
	// only when user not specified custom value
	if(m_bpm != 0)
	{
		BOOST_LOG_TRIVIAL(info) << "BPM set to user custom value: " << m_bpm;
	}
	else
	{
		std::vector<Event> events = track.getEvents();
		for(auto event : events)
		{
			if(event.getCmd().getFullCmd() == TEMPO)
			{
				m_bpm = EventTempo(event).get();
				BOOST_LOG_TRIVIAL(info) << "Found original BPM information in service track: " << m_bpm;
				break;
			}
		}
	}
	if(m_bpm == 0)
	{
		m_bpm = g_default_bpm;
		BOOST_LOG_TRIVIAL(info) << "Cannot extract track BPM, setting default value: " << m_bpm;
	}

	for(uint64_t i=0; i<m_mtrkTracks.size(); i++)
		m_mtrkTracks[i].updateBpm(m_bpm);

	return 0;
}

int MidiFile::processV2()
{
	return 0;
}

int MidiFile::postProcess()
{
	int ret = 0;
	switch (m_mthd.getFormatType())
	{
	case MIDI_V0:
		ret = processV0();
		break;
	case MIDI_V1:
		ret = processV1();
		break;
	case MIDI_V2:
		ret = processV2();
		break;
	default:
		BOOST_LOG_TRIVIAL(error) << "Invalid MThd format";
		ret = -1;
		break;
	}
	return ret;
}

int MidiFile::process()
{
	if(m_filePath == "")
		return -1;

	ByteStream stream(m_filePath);
	if(!stream.isOk())
	{
		BOOST_LOG_TRIVIAL(error) << "Stream is invalid";
		return -1;
	}
	m_mthd = MthdHeader(stream);
	m_mthd.log();
	if(!m_mthd.isOk())
	{
		BOOST_LOG_TRIVIAL(error) << "MThd header is invalid";
		return -1;
	}
	for(uint64_t i=0; i<m_mthd.getChunksCount(); i++)
	{
		MtrkHeader mtrk(stream, m_bpm, m_mthd.getPPQN());
		mtrk.log();
		if(!mtrk.isOk())
		{
			BOOST_LOG_TRIVIAL(error) << "MTrk chunk header is invalid";
			return -1;
		}
		m_mtrkTracks.push_back(mtrk);
	}
	return postProcess();
}

std::vector<MtrkHeader> MidiFile::getTracks()
{
	switch (m_mthd.getFormatType())
	{
	case MIDI_V0:
		return m_mtrkTracks;
	case MIDI_V1:
	{
		// In format 1 we need to ignore track under index zero
		// because it is service track and it need to be without notes
		std::vector<MtrkHeader> filteredTracks(m_mtrkTracks);
		filteredTracks.erase(filteredTracks.begin());
		return filteredTracks;
	}
	case MIDI_V2:
		return m_mtrkTracks;
	default:
		return std::vector<MtrkHeader>();
	}
}

uint16_t MidiFile::getVersion()
{
	return m_mthd.getFormatType();
}
