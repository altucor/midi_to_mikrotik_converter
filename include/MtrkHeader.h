#ifndef MTRK_HEADER_HPP
#define MTRK_HEADER_HPP

#include "ByteStream.h"
#include "VLV.h"
#include "MidiEvent.h"

const static std::string g_mtrk_reference = "MTrk";

class MtrkHeader
{
public:
	MtrkHeader(ByteStream &stream, double pulsesPerSec);
	~MtrkHeader();
	void log();
	bool isOk();
	bool isServiceTrack();
	uint64_t getStartPos();
	uint64_t getEndPos();
	uint64_t getSize();
	std::vector<MidiEvent> getEvents();
	std::vector<Note> getNotes();
private:
	double m_pulsesPerSec = 0.0;
	std::string m_mtrk;
	uint32_t m_size = 0;
	uint64_t m_startPos = 0;
	uint64_t m_endPos = 0;
	VLV m_track_predelay;
	std::vector<MidiEvent> m_midiEvents;
	std::vector<Note> m_midiNotes;
};

#endif // MTRK_HEADER_HPP
