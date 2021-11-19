#include "MidiEvent.h"
#include "boost/log/trivial.hpp"

#include <iostream>
#include <utility>
#include <iomanip>
#include <sstream>

MidiEvent::MidiEvent(uint8_t cmd, ByteStream &stream, double pulsesPerSec)
{
	m_pulsesPerSec = pulsesPerSec;
	m_ok = false;
	m_cmd = cmd;
RESTART_MARKER:
	switch (m_cmd)
	{
	case READ_NEXT_CMD:
	case SYSTEM_RESET:
		m_cmd = stream.get8u();
		goto RESTART_MARKER;
	default:
		break;
	}
	m_cmd_size = VLV(stream);
	m_data = stream.getDataPart(m_cmd_size.getValue());
	m_ok = true;
}

MidiEvent::~MidiEvent()
{
}

void printHexBuffer(std::vector<uint8_t> &buf)
{
	if(buf.size() == 0)
		return;
	std::stringstream ss;
	for(std::size_t i=0; i<buf.size(); i++)
	{
		ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((int)buf[i]) << " " << std::dec;
	}
	std::cout << ss.str();
	std::cout << "\n";
}

std::stringstream toPrintHex(uint64_t val, size_t width)
{
	std::stringstream ss;
	ss << "0x" <<
		std::hex << 
		std::uppercase << 
		std::setfill('0') << 
		std::setw(width) << 
		((int64_t)val) << 
		std::dec;
	return ss;
}

void MidiEvent::log()
{
	BOOST_LOG_TRIVIAL(info) << "MIDI cmd: " << toPrintHex(m_cmd, 1).str()
	<< " data size: " << (uint32_t)m_cmd_size.getValue();
	printHexBuffer(m_data);
}

bool MidiEvent::isOk()
{
	return m_ok;
}

uint8_t MidiEvent::getType()
{
	return m_cmd;
}

std::vector<uint8_t> MidiEvent::getData()
{
	return m_data;
}
