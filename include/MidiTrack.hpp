#ifndef MIDI_TRACK_HPP
#define MIDI_TRACK_HPP

#include "ByteStream.hpp"
#include "VLV.hpp"
#include "Event.hpp"
#include "MidiEvent.hpp"
#include "MidiMetaEvent.hpp"

const static std::string g_mtrk_reference = "MTrk";

class MidiTrack
{
public:
	MidiTrack() {};
	MidiTrack(ByteStream &stream, const uint16_t bpm, const uint16_t ppqn);
	void log();
	bool isOk();
	std::vector<Event> getEvents();
	double getPulsesPerSecond();
	double getPreDelayMs();
	void updateBpm(const uint16_t bpm);
	uint16_t getBpm();
	std::string getName();
	std::string getInstrumentName();
private:
	uint16_t m_bpm = 0;
	uint16_t m_ppqn = 0;
	std::string m_mtrk;
	uint32_t m_size = 0;
	uint64_t m_startPos = 0;
	uint64_t m_endPos = 0;
	VLV m_trackPreDelay;
	std::vector<Event> m_events;
	std::string m_trackName;
	std::string m_instrumentName;
};

#endif // MIDI_TRACK_HPP
