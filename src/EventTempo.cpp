#include "EventTempo.hpp"


EventTempo::EventTempo(Event &event)
{
	//FF 51 03 tt tt tt
	std::vector<uint8_t> data = event.getData();
	m_tempo = data[0] << 16;
	m_tempo |= data[1] << 8;
	m_tempo |= data[2];
	m_tempo = 60000000 / m_tempo;
}

EventTempo::~EventTempo()
{
}

uint32_t EventTempo::get()
{
	return m_tempo;
}
