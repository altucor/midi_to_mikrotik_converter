#ifndef MTRK_HEADER_HPP
#define MTRK_HEADER_HPP

#include "../include/ByteStream.h"
#include "../include/MidiEvent.h"

const static std::string g_mtrk_reference = "MTrk";

class MtrkHeader
{
public:
	MtrkHeader(ByteStream &stream);
	~MtrkHeader();
	void log();
	bool isOk();
	uint64_t getStartPos();
	uint64_t getEndPos();
	uint64_t getSize();
private:
	std::string m_mtrk;
	uint32_t m_size = 0;
	uint64_t m_startPos = 0;
	uint64_t m_endPos = 0;
	std::vector<MidiEvent> m_midiEvents;
};

#endif // MTRK_HEADER_HPP
