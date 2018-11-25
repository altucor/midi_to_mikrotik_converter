#ifndef MTRKCHUNK_H
#define MTRKCHUNK_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

enum STATE
{
    UNDEFINED = 0,
    HEADER_MARKER,
    HEADER_SIZE,
    FIRST_DELAY,
    NOTE_ON,
    NOTE_OFF,
    EVENT,
    TEXT,
    COPYRIGHT,
    TRACK_NAME,
    INSTRUMENT_NAME,
    VOCAL_TEXT,
    TEXT_MARKER,
    CUE_POINT,
    MIDI_CHANNEL,
    MIDI_PORT,
    TEMPO,
    SMPTE_OFFSET,
    TIME_SIGNATURE,
    KEY_SIGNATURE,
    TRACK_END,
    UNPREDICTED,
    UNKNOWN_BYTE
};

const static uint8_t eventMarker = 0xFF;

struct NoteCmd
{
    uint8_t channel : 4;
	uint8_t cmd : 4;
};

struct NoteEvent
{
    NoteCmd noteCmd;
    uint8_t pitch = 0;
    uint8_t velocity = 0;
    std::vector<uint8_t> length;
};

struct HumanizedNote
{
    std::string symbolicCode = "";
    double frequency = 0.0;
    std::vector<uint8_t> durationOn;
    std::vector<uint8_t> durationOff;
};

struct TimeSignature
{
	uint8_t nn = 0;
	uint8_t dd = 0;
	uint8_t cc = 0;
	uint8_t bb = 0;
};

class MTrkChunk
{
    public:
        // Functions
        explicit MTrkChunk();
        ~MTrkChunk();
        void initialize(std::vector<uint8_t> &fileData, uint64_t startPos, uint64_t endPos);
        void setOctaveShift(int octaveShift);
        void setNoteShift(int noteShift);
        void setDebugLevel(int debugLevel);
        int8_t getMidiChannel();
        int32_t getNotesCount();
        std::vector<HumanizedNote> getHumanizedNotes();
        int process();
		int64_t getStartPos();
		int64_t getEndPos();
		int64_t getSize();
        int getTempo();
        void dbg_printBody();
        std::string getTrackName();
        std::string getTrackText();
        //vector<int> getFirstDelay();


    private:
        void m_stateMachine();
        int m_work();
        int m_getChannelFromCmd(uint8_t cmd);
        void m_debugPrintNoteEvent(NoteEvent note);
        int m_detectNoteEvents(uint8_t cmdMask, std::vector<NoteEvent> &noteEvents);
        void m_debugPrintHumanizedNote(HumanizedNote hNote);
        void m_mapEventsToHumanizedNotes();
        void m_detectTempo();
        void m_detectTrackName();
        void m_detectTrackText();
        bool m_isTrackEnd(uint64_t startIndex);
        bool m_isNoteEnd(uint64_t startIndex);
        //void m_detect_first_delay();

    private:
        STATE m_cms = UNDEFINED; // Current Machine State
        std::vector<NoteEvent> m_notesOn;
        std::vector<NoteEvent> m_notesOff;
		uint8_t m_channel = -1;
        int m_cmdNoteOnMask  = 0x09; // 0x90
        int m_cmdNoteOffMask =  0x08; //0x80
        int m_chunkPtr = 0;
        std::vector<uint8_t> m_chunkData;
        int32_t m_midiChannel = 0;
        int m_mtrkOctaveShift = 0;
        int m_mtrkNoteShift = 0;
        int64_t m_mtrkStartPos = 0;
		int64_t m_mtrkEndPos = 0;
		int64_t m_mtrkSize = 0;
        int32_t m_tempo = 0;
		TimeSignature m_timeSignature;
        int m_debugLevel = 0;
        std::vector<uint8_t> m_firstDelays;
        std::string m_trackName = "";
        std::string m_trackText = "";
        std::vector<HumanizedNote> m_humanizedNotes;
};

#endif // MTRKCHUNK_H
