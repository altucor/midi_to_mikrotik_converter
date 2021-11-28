#ifndef MIDI_META_EVENT_HPP
#define MIDI_META_EVENT_HPP

#include "ByteStream.h"
#include "MidiEventCode.h"
#include "VLV.h"

class MidiMetaEvent
{
public:
	MidiMetaEvent(ByteStream &stream);
	~MidiMetaEvent();
	void log();
	std::vector<uint8_t> getData();
	uint8_t getCmd();
private:
	MidiEventCode m_cmd;
	VLV m_dataSize;
	std::vector<uint8_t> m_data;
	VLV m_delay;
};


#endif // MIDI_META_EVENT_HPP
