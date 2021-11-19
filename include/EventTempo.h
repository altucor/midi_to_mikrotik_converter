#ifndef EVENT_TEMPO_HPP
#define EVENT_TEMPO_HPP

#include "MidiEvent.h"

class EventTempo
{
public:
	EventTempo(MidiEvent &event);
	~EventTempo();
	uint32_t get();
private:
	std::vector<uint8_t> m_data;
	uint32_t m_tempo = 0;
};




#endif // EVENT_TEMPO_HPP