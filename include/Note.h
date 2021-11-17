#ifndef NOTE_HPP
#define NOTE_HPP

#include "../include/ByteStream.h"
#include <vector>

enum NOTE_CMD
{
	NOTE_OFF,
	NOTE_ON
};

class Note
{
public:
	Note(ByteStream &stream, uint8_t cmd);
	~Note();
private:
	NOTE_CMD note_cmd;
	uint8_t velocity;
	std::vector<uint8_t> vlv_duration;
	/*
	 * Very detailed article about VLV(Variable Length Values)
	 * http://www.ccarh.org/courses/253/handout/vlv/
	 * Thats why 0x7F and -1 at the end
	 */
};




#endif // NOTE_HPP