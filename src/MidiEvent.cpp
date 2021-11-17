#include "../include/MidiEvent.h"
#include "../include/Note.h"
#include "boost/log/trivial.hpp"

#include <iostream>
#include <utility>
#include <iomanip>
#include <sstream>

MidiEvent::MidiEvent(ByteStream &stream)
{
	m_ok = false;
	m_cmd = stream.get8u();
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
	if(m_cmd >= 0x80 && m_cmd <= 0x9F)
	{
		Note note(stream, m_cmd);
	}
	else
	{
		m_cmd_size = stream.get8u();
		m_data = stream.getDataPart(m_cmd_size);
		m_ok = true;
	}
}

MidiEvent::~MidiEvent()
{
}

void printHexBuffer(std::vector<uint8_t> &buf)
{
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
	BOOST_LOG_TRIVIAL(info) << "MIDI cmd: " << toPrintHex(m_cmd, 1).str();
	BOOST_LOG_TRIVIAL(info) << "MIDI command size: " << (uint32_t)m_cmd_size;
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


