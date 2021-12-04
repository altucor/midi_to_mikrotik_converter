#ifndef MIDI_EVENT_HPP
#define MIDI_EVENT_HPP

#include "ByteStream.hpp"
#include "MidiEventCode.hpp"
#include "VLV.hpp"
#include <vector>

class MidiEvent
{
public:
	MidiEvent(MidiEventCode cmd, ByteStream &stream);
	~MidiEvent();
	void log();
	MidiEventCode getCmd();
	uint8_t getSecondByte();
	uint8_t getThirdByte();
	VLV getDelay();
private:
	MidiEventCode m_cmd;
	uint8_t m_byte2;
	uint8_t m_byte3;
	VLV m_delay;
};

#endif // MIDI_EVENT_HPP
