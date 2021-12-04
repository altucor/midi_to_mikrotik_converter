#include "MidiEvent.hpp"
#include "Utils.hpp"
#include "boost/log/trivial.hpp"

MidiEvent::MidiEvent(MidiEventCode cmd, ByteStream &stream)
	: m_cmd(cmd)
{
	uint8_t mainCmd = m_cmd.getMainCmd();
	m_byte2 = stream.get8u();
	// http://www33146ue.sakura.ne.jp/staff/iz/formats/midi-event.html
	// Some midi events consist of 2 or 3 bytes
	switch (mainCmd)
	{
	case NOTE_OFF:
	case NOTE_ON:
	case POLYPHONIC_AFTERTOUCH_PRESSURE:
	case CONTROL_MODE_CHANGE:
	case PROGRAM_CHANGE:
	case PITCH_WHEEL:
		m_byte3 = stream.get8u();
		break;
	default:
		break;
	}
	m_delay = VLV(stream);
}

MidiEvent::~MidiEvent()
{
}

void MidiEvent::log()
{
	BOOST_LOG_TRIVIAL(info) 
	<< "Full cmd: " << (uint32_t)m_cmd.getFullCmd()
	<< " byte2: " << (uint32_t)m_byte2
	<< " byte3: " << (uint32_t)m_byte3
	<< " duration value: " << (uint32_t)m_delay.getValue();
}

MidiEventCode MidiEvent::getCmd()
{
	return m_cmd;
}

uint8_t MidiEvent::getSecondByte()
{
	return m_byte2;
}

uint8_t MidiEvent::getThirdByte()
{
	return m_byte3;
}

VLV MidiEvent::getDelay()
{
	return m_delay;
}
