#ifndef NOTE_HPP
#define NOTE_HPP

#include "ByteStream.h"
#include "VLV.h"
#include <vector>

enum NOTE_CMD
{
	NOTE_UNKNOWN = 0,
	NOTE_OFF = 8,
	NOTE_ON = 9
};

class Note
{
public:
	Note();
	Note(uint8_t cmd, ByteStream &stream, double m_pulsesPerSecond);
	~Note();
	void log();
	NOTE_CMD getType();
	void setOctaveShift(const int shift);
	void setNoteShift(const int shift);
	double getFreqencyHz();
	std::string getSymbolicNote();
	double getDurationPulses();
private:
	int m_octaveShift = 0;
	int m_noteShift = 0;
	double m_pulsesPerSecond = 0.0;
	uint8_t m_cmd = 0;
	uint8_t m_channel = 0;
	uint8_t m_pitch = 0;
	uint8_t m_velocity = 0;
	VLV m_vlv;
};


#endif // NOTE_HPP
