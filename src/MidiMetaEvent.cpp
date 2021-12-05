#include "MidiMetaEvent.hpp"
#include "Utils.hpp"
#include "boost/log/trivial.hpp"

MidiMetaEvent::MidiMetaEvent(ByteStream &stream)
{
	m_cmd = MidiEventCode(stream.get8u());
	m_dataSize = VLV(stream);
	m_data = stream.getDataPart(m_dataSize.getValue());
	m_delay = VLV(stream);
}

void MidiMetaEvent::log()
{
	BOOST_LOG_TRIVIAL(info) << "MIDI cmd: " << Utils::toHex(m_cmd.getMainCmd(), 1)
	<< " sub-cmd: " << Utils::toHex(m_cmd.getSubCmd(), 1)
	<< " data size: " << (uint32_t)m_dataSize.getValue();
	BOOST_LOG_TRIVIAL(info) << "MIDI data: " << Utils::toHexBuffer(m_data);
}

std::vector<uint8_t> MidiMetaEvent::getData()
{
	return m_data;
}

uint8_t MidiMetaEvent::getCmd()
{
	return m_cmd.getFullCmd();
}
