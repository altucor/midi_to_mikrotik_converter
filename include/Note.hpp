#ifndef NOTE_HPP
#define NOTE_HPP

#include "VLV.hpp"
#include "Event.hpp"
#include <vector>

enum NOTE_TYPE
{
	NOTE_TYPE_UNKNOWN = 0,
	NOTE_TYPE_OFF = 8,
	NOTE_TYPE_ON = 9
};

class Note
{
public:
	Note() {};
	Note(Event &event);
	void log();
	NOTE_TYPE getType();
	double getFrequencyHz(
		const int octaveShift, 
		const int noteShift, 
		const double fineTuning
	);
	std::string getSymbolicNote(
		const int octaveShift, 
		const int noteShift, 
		const double fineTuning
	);
	VLV getDelay();
private:
	uint8_t m_cmd = 0;
	uint8_t m_channel = 0;
	uint8_t m_pitch = 0;
	uint8_t m_velocity = 0;
	VLV m_delay;
};

#endif // NOTE_HPP
