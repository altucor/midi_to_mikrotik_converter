#ifndef EVENT_HPP
#define EVENT_HPP

#include "ByteStream.hpp"
#include "MidiEventCode.hpp"
#include "VLV.hpp"
#include <vector>

class Event
{
public:
	Event(ByteStream &stream);
	~Event();
	void log();
	MidiEventCode getCmd();
	std::vector<uint8_t> getData();
	VLV getDelay();
private:
	MidiEventCode m_cmd;
	std::vector<uint8_t> m_data;
	VLV m_delay;
};

#endif // EVENT_HPP
