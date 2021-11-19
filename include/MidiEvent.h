#ifndef MIDI_EVENT_HPP
#define MIDI_EVENT_HPP

#include "ByteStream.h"
#include "VLV.h"
#include "Note.h"
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
	CHANNEL_1_NOTE_OFF = 0x80,
	/*
		0x80 - Channel 1 Note Off
		...
		0x8F - Channel 1 Note Off
		0x90 - Channel 1 Note On
		...
		0x9F - Channel 1 Note On
	*/
	CHANNEL_16_NOTE_ON = 0x9F,
	SYSEX = 0xF0,
	SYSTEM_RESET = 0xFF,
};

class MidiEvent
{
public:
	MidiEvent(uint8_t cmd, ByteStream &stream, double pulsesPerSec);
	~MidiEvent();
	void log();
	bool isOk();
	uint8_t getType();
	std::vector<uint8_t> getData();
private:
	double m_pulsesPerSec = 0.0;
	bool m_ok;
	uint8_t m_cmd = 0;
	VLV m_cmd_size;
	std::vector<uint8_t> m_data;
};

#endif // MIDI_EVENT_HPP
