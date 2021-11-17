#include "../include/MthdHeader.h"
#include "boost/log/trivial.hpp"

MthdHeader::MthdHeader(ByteStream &stream)
{
	m_mthd = stream.get_text_header();
	m_mthdChunkLength = stream.get32u_bswap();
	m_formatType = stream.get16u_bswap();
	m_mtrkChunksCount = stream.get16u_bswap();
	m_ppqn = stream.get16u_bswap();
}
MthdHeader::~MthdHeader()
{

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
	return g_mthd_reference == m_mthd;
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
