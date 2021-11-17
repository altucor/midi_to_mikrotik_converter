#ifndef MIDI_EVENT_HPP
#define MIDI_EVENT_HPP

#include "../include/ByteStream.h"
#include <vector>

enum EVENTS
{
	READ_NEXT_CMD = 0x00,
	TEXT = 0x01,
	COPYRIGHT = 0x02,
	TRACK_NAME = 0x03,
	INSTRUMENT_NAME = 0x04,
	VOCAL_TEXT = 0x05,
	TEXT_MARKER = 0x06,
	CUE_POINT = 0x07,
	MIDI_CHANNEL = 0x20,
	MIDI_PORT = 0x21,
	TEMPO = 0x51,
	SMPTE_OFFSET = 0x54,
	TIME_SIGNATURE = 0x58,
	KEY_SIGNATURE = 0x59,
	TRACK_END = 0x2F,
	SYSTEM_RESET = 0xFF,
};

class MidiEvent
{
public:
	MidiEvent(ByteStream &stream);
	~MidiEvent();
	void log();
	bool isOk();
	uint8_t getType();
private:
	bool m_ok;
	uint8_t m_cmd;
	uint8_t m_cmd_size;
	std::vector<uint8_t> m_data;
};

#endif // MIDI_EVENT_HPP
