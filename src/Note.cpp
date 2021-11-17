#include "../include/Note.h"

const static uint8_t MIDI_VLV_CONTINUATION_BIT = 0x80;

Note::Note(ByteStream &stream, uint8_t cmd)
{
	uint8_t number = stream.get8u(); // note number
	uint8_t velocity = stream.get8u(); // note velocity
	uint8_t duration = stream.get8u(); // note duration
	while(duration >= MIDI_VLV_CONTINUATION_BIT)
	{
		duration = stream.get8u(); // note duration
	}
}

Note::~Note()
{
}
