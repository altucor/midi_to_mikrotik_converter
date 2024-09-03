#ifndef MIKROTIK_HPP
#define MIKROTIK_HPP

#include "mtrk.h"
#include "Note.hpp"
#include <vector>

class Mikrotik
{
public:
	Mikrotik(mtrk_t *track,
			 const uint16_t ppqn,
			 const int32_t bpm,
			 const uint64_t index,
			 const int octaveShift,
			 const int noteShift,
			 const double fineTuning,
			 const bool commentsFlag);
	int buildScriptForChannel(std::string &fileName, const uint8_t channel);
	int buildScript(std::string &fileName);

private:
	std::string getTimeAsText(const double time);
	std::string getTrackTimeLength(const uint8_t channel);
	std::string getNotesCount(const uint8_t channel);
	std::string getScriptHeader(const uint8_t channel);
	std::string getDelayLine(const float delayMs);
	std::string getBeepLine(midi_note_t note, const float duration);
	// std::string buildNote(Note note);

private:
	bool m_commentsFlag = false;
	uint16_t m_ppqn;
	int32_t m_bpm;
	uint64_t m_index = 0;
	int m_octaveShift = 0;
	int m_noteShift = 0;
	double m_fineTuning = 0.0;
	double m_processedTime = 0.0;
	mtrk_t *m_track;
};

#endif // MIKROTIK_HPP
