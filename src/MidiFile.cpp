#include "MidiFile.h"
#include "ByteStream.h"
#include "MidiEvent.h"
#include "EventTempo.h"

#include <boost/log/trivial.hpp>
#include <fstream>

const static uint8_t CONTINUATION_BIT = 0x80;

MidiFile::MidiFile(std::string &filePath, const int octaveShift, const int noteShift, const int bpm)
	: m_filePath(filePath), m_octaveShift(octaveShift), m_noteShift(noteShift), m_bpm(bpm)
{

}

MidiFile::~MidiFile()
{
	
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
		MtrkHeader mtrk(stream, m_pulsesPerSec);
		mtrk.log();
		if(!mtrk.isOk())
		{
			BOOST_LOG_TRIVIAL(error) << "MTrk chunk header is invalid";
			return -1;
		}
		if(mtrk.isServiceTrack())
		{
			// Read BPM from MIDI File only when user not
			// specified custom value
			

			if(m_bpm != 0)
			{
				BOOST_LOG_TRIVIAL(info) << "BPM set to user custom value: " << m_bpm;
			}
			else
			{
				std::vector<MidiEvent> events = mtrk.getEvents();
				for(auto event : events)
				{
					if(event.getType() == TEMPO)
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
			else
			{
				
			}
			m_pulsesPerSec = (double)60000.0 / ( m_bpm * m_mthd.getPPQN() );
			BOOST_LOG_TRIVIAL(info) << "Pulses per second: " << m_pulsesPerSec;
		}
		m_mtrkTracks.push_back(mtrk);
	}
	return 0;
}

std::vector<MtrkHeader> MidiFile::getTracks()
{
	return m_mtrkTracks;
}
