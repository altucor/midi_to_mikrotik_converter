#ifndef MTHD_HEADER_HPP
#define MTHD_HEADER_HPP

#include "../include/ByteStream.h"

const static std::string g_mthd_reference = "MThd";

class MthdHeader
{
public:
	MthdHeader(ByteStream &stream);
	~MthdHeader();
	void log();
	bool isOk();
	uint32_t getChunkLength();
	uint16_t getFormatType();
	uint16_t getChunksCount();
	uint16_t getPPQN();
private:
	std::string m_mthd;
	uint32_t m_mthdChunkLength = 0;
	uint16_t m_formatType = 0;
	uint16_t m_mtrkChunksCount = 0;
	uint16_t m_ppqn = 0;
};

#endif // MTHD_HEADER_HPP
