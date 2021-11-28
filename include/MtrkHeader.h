#ifndef MTRK_HEADER_HPP
#define MTRK_HEADER_HPP

#include "ByteStream.h"
#include "VLV.h"
#include "Event.h"
#include "MidiEvent.h"
#include "MidiMetaEvent.h"

const static std::string g_mtrk_reference = "MTrk";

class MtrkHeader
{
public:
	MtrkHeader();
	MtrkHeader(ByteStream &stream, const uint16_t bpm, const uint16_t ppqn);
	~MtrkHeader();
	void log();
	bool isOk();
	std::vector<Event> getEvents();
	uint64_t getStartPos();
	uint64_t getEndPos();
	uint64_t getSize();
	double getPulsesPerSecond();
	double getPreDelayMs();
	void updateBpm(const uint16_t bpm);
	uint16_t getBpm();
	std::string getName();
	std::string MtrkHeader::getInstrumentName();
private:
	uint16_t m_bpm = 0;
	uint16_t m_ppqn = 0;
	std::string m_mtrk;
	uint32_t m_size = 0;
	uint64_t m_startPos = 0;
	uint64_t m_endPos = 0;
	VLV m_trackPreDelay;
	std::vector<Event> m_events;
};

#endif // MTRK_HEADER_HPP
