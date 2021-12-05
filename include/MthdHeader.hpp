#ifndef MTHD_HEADER_HPP
#define MTHD_HEADER_HPP

#include "ByteStream.hpp"

const static std::string g_mthd_reference = "MThd";

enum MIDI_FORMAT_VERSIONS
{
	MIDI_V0, // Single track SMF
	MIDI_V1, // Multi-Track SMF, with first track reserved for service commands
	MIDI_V2,
};

class MthdHeader
{
public:
	MthdHeader() {};
	MthdHeader(ByteStream &stream);
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
	// PPQN - Pulses Per Quarter Note
	uint16_t m_ppqn = 0;
};

#endif // MTHD_HEADER_HPP
