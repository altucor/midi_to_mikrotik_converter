#ifndef MIDI_EVENT_CODE_HPP
#define MIDI_EVENT_CODE_HPP

#include <stdint.h>

// High part of 4 bits
enum MIDI_EVENT_MAIN_CMD : uint8_t
{
	UNKNOWN = 0x00,
	UNK_1 = 0x01,
	UNK_2 = 0x02,
	UNK_3 = 0x03,
	UNK_4 = 0x04,
	UNK_5 = 0x05,
	UNK_6 = 0x06,
	UNK_7 = 0x07,
	NOTE_OFF = 0x08, // low 4 bits - channel
	NOTE_ON = 0x09, // low 4 bits - channel
	POLYPHONIC_AFTERTOUCH_PRESSURE = 0x0A, // low 4 bits - channel
	CONTROL_MODE_CHANGE = 0x0B, // low 4 bits - channel
	PROGRAM_CHANGE = 0x0C, // low 4 bits - channel
	CHANNEL_AFTERTOUCH_PRESSURE = 0x0D, // low 4 bits - channel
	PITCH_WHEEL = 0x0E, // low 4 bits - channel
	SYSTEM_EVENT = 0x0F, // has sub-commands
};

// Low 4 bits of SYSTEM_EVENT command
enum MIDI_SYSTEM_EVENTS: uint8_t
{
	SYS_EXCLUSIVE = 0x00,
	SYS_MIDI_TIME_CODE_QUARTER_FRAME = 0x01,
	SYS_SONG_POSITION_POINTER = 0x02,
	SYS_SONG_SELECT = 0x03,
	SYS_RESERVED_4 = 0x04,
	SYS_RESERVED_5 = 0x05,
	SYS_TUNE_REQUEST = 0x06,
	SYS_END_OF_SYSEX = 0x07, // EOX
	SYS_REAL_TIME_TIMING_CLOCK = 0x08,
	SYS_REAL_TIME_UNDEFINED_9 = 0x09,
	SYS_REAL_TIME_START = 0x0A,
	SYS_REAL_TIME_CONTINUE = 0x0B,
	SYS_REAL_TIME_STOP = 0x0C,
	SYS_REAL_TIME_UNDEFINED_D = 0x0D,
	SYS_REAL_TIME_ACTIVE_SENSING = 0x0E,
	SYS_META_EVENT = 0x0F
};

enum MIDI_META_EVENTS : uint8_t
{
	SEQUENCE_NUMBER = 0x00,
	TEXT = 0x01,
	COPYRIGHT = 0x02,
	TRACK_NAME = 0x03,
	INSTRUMENT_NAME = 0x04,
	LYRIC_TEXT = 0x05, // A single Lyric MetaEvent should contain only one syllable
	TEXT_MARKER = 0x06,
	CUE_POINT = 0x07,
	PROGRAM_PATCH_NAME = 0x08,
	DEVICE_PORT_NAME = 0x09,
	MIDI_CHANNEL = 0x20,
	MIDI_PORT = 0x21,
	TRACK_END = 0x2F,
	TEMPO = 0x51,
	SMPTE_OFFSET = 0x54,
	TIME_SIGNATURE = 0x58,
	KEY_SIGNATURE = 0x59,
	PROPRIETARY_EVENT = 0x7F,
};

class MidiEventCode
{
public:
	MidiEventCode() {};
	MidiEventCode(const uint8_t cmd);
	uint8_t getMainCmd();
	uint8_t getSubCmd();
	uint8_t getFullCmd();
	bool isMetaEvent();
private:
	uint8_t m_fullCmd = 0;
	uint8_t m_mainCmd = 0;
	uint8_t m_subCmd = 0;
};

#endif // MIDI_EVENT_CODE_HPP
