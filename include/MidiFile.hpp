#ifndef MIDIFILE_H
#define MIDIFILE_H

#include "MthdHeader.hpp"
#include "MidiTrack.hpp"

#include <string>
#include <vector>

// MIDI Spec: If there are no tempo events in a MIDI file, then the tempo is assumed to be 120 BPM
const uint8_t g_default_bpm = 120;

class MidiFile
{
public:
	explicit MidiFile(
		std::string &filePath, 
		const uint32_t bpm
	) : m_filePath(filePath), m_bpm(bpm) {};
	int process();
	std::vector<MidiTrack> getTracks();
	uint16_t getVersion();
private:
	MthdHeader m_mthd;
	uint16_t m_bpm = 0;
	std::string m_filePath;
	std::vector<MidiTrack> m_mtrkTracks;
private:
	int processV0();
	int processV1();
	int processV2();
	int postProcess();
};

#endif // MIDIFILE_H
