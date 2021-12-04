#include "MidiEventCode.hpp"

MidiEventCode::MidiEventCode()
{

}

MidiEventCode::MidiEventCode(const uint8_t cmd)
{
	m_fullCmd = cmd;
	m_mainCmd = ((cmd >> 4) & 0x0F);
	m_subCmd = (cmd & 0x0F);
}

MidiEventCode::~MidiEventCode()
{
}

uint8_t MidiEventCode::getMainCmd()
{
	return m_mainCmd;
}

uint8_t MidiEventCode::getSubCmd()
{
	return m_subCmd;
}

uint8_t MidiEventCode::getFullCmd()
{
	return m_fullCmd;
}

bool MidiEventCode::isMetaEvent()
{
	return m_mainCmd == SYSTEM_EVENT && m_subCmd == SYS_META_EVENT;
}
