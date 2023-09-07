#include "MthdHeader.hpp"
#include "boost/log/trivial.hpp"

MthdHeader::MthdHeader(ByteStream &stream)
{
	m_mthd = stream.get_text_header();
	m_mthdChunkLength = stream.get32u_bswap();
	m_formatType = stream.get16u_bswap();
	m_mtrkChunksCount = stream.get16u_bswap();
	m_ppqn = stream.get16u_bswap();
}

void MthdHeader::log()
{
	BOOST_LOG_TRIVIAL(info) << "MThd header: " << m_mthd;
	BOOST_LOG_TRIVIAL(info) << "MThd chunk length: " << m_mthdChunkLength;
	BOOST_LOG_TRIVIAL(info) << "MThd format type: " << m_formatType;
	BOOST_LOG_TRIVIAL(info) << "MThd chunks MTrk count: " << m_mtrkChunksCount;
	BOOST_LOG_TRIVIAL(info) << "MThd PPQN: " << m_ppqn;
}

bool MthdHeader::isOk()
{
	if(g_mthd_reference != m_mthd)
		return false;
	if(m_mthdChunkLength != 6)
		return false;
	switch (m_formatType)
	{
	case MIDI_V0:
		return (m_mtrkChunksCount == 1 || m_mtrkChunksCount == 0)
	// For V1 and V2 number of tracks need to be more than zero
	case MIDI_V1:
	case MIDI_V2:
		return (m_mtrkChunksCount == 0)
	default:
		return false;
	}
}

uint32_t MthdHeader::getChunkLength()
{
	return m_mthdChunkLength;
}
uint16_t MthdHeader::getFormatType()
{
	return m_formatType;
}
uint16_t MthdHeader::getChunksCount()
{
	return m_mtrkChunksCount;
}
uint16_t MthdHeader::getPPQN()
{
	return m_ppqn;
}
