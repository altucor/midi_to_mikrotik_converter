#ifndef MIDIFILE_H
#define MIDIFILE_H

#include "MthdHeader.h"
#include "MTrkHeader.h"

#include <string>
#include <vector>

struct TrackLength
{
	uint32_t h = 0;
	uint32_t m = 0;
	uint32_t s = 0;
	double ms = 0;
};

// MIDI Spec: If there are no tempo events in a MIDI file, then the tempo is assumed to be 120 BPM
const uint8_t g_default_bpm = 120;

class MidiFile
{
public:
	explicit MidiFile(std::string &filePath, const int octaveShift, const int noteShift, const int bpm);
	~MidiFile();
	int process();
	std::vector<MtrkHeader> getTracks();
private:
	MthdHeader m_mthd;
	int m_octaveShift = 0;
	int m_noteShift = 0;
	uint16_t m_bpm = 0;
	double m_pulsesPerSec = 0.0;
	std::string m_filePath;
	std::vector<MtrkHeader> m_mtrkTracks;
};

#endif // MIDIFILE_H
