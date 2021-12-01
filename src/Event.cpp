#include "Event.h"
#include "Utils.h"
#include "boost/log/trivial.hpp"
#include <iomanip>

// http://midi.teragonaudio.com/tech/midifile.htm
// Very good examples and description about midi meta events

Event::Event(ByteStream &stream)
{
	m_cmd = MidiEventCode(stream.get8u());
	if(m_cmd.isMetaEvent())
	{
		m_cmd = MidiEventCode(stream.get8u());
		VLV dataSize(stream);
		m_data = stream.getDataPart(dataSize.getValue());
		if(m_cmd.getFullCmd() == TRACK_END && dataSize.getValue() == 0)
			return;
	}
	else
	{
		m_data.push_back(stream.get8u());
		// http://www33146ue.sakura.ne.jp/staff/iz/formats/midi-event.html
		// Some midi events consist of 2 or 3 bytes
		uint8_t mainCmd = m_cmd.getMainCmd();
		switch (mainCmd)
		{
		case NOTE_OFF:
		case NOTE_ON:
		case POLYPHONIC_AFTERTOUCH_PRESSURE:
		case CONTROL_MODE_CHANGE:
		case PITCH_WHEEL:
			m_data.push_back(stream.get8u());
			break;
		default:
			break;
		}
	}
	m_delay = VLV(stream);
}

Event::~Event()
{
}

void Event::log()
{
	BOOST_LOG_TRIVIAL(info) 
	<< "Event cmd: " << Utils::toHex(m_cmd.getFullCmd(), 2)
	<< " delay vlv: " 
	<< std::setfill('0')
	<< std::setw(4)
	<<  m_delay.getValue()
	<< " data size: " << m_data.size() 
	<< " data: " << Utils::toHexBuffer(m_data);
}

MidiEventCode Event::getCmd()
{
	return m_cmd;
}

std::vector<uint8_t> Event::getData()
{
	return m_data;
}

VLV Event::getDelay()
{
	return m_delay;
}

