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
    UNKNOWN_BYTE,
	PARSE_BYTE_COMMAND,
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

struct KeySignature
{
	uint8_t sf = 0;
	uint8_t mi = 0;
};

struct SMPTEOffset
{
	uint8_t hr = 0;
	uint8_t mn = 0;
	uint8_t se = 0;
	uint8_t fr = 0;
	uint8_t ff = 0;
};

class MTrkChunk
{
    public:
        // Public interface
        explicit MTrkChunk();
        ~MTrkChunk();
        void initialize(std::vector<uint8_t> &fileData, uint64_t startPos, uint64_t endPos);
        void setOctaveShift(int octaveShift);
        void setNoteShift(int noteShift);
        void setDebugLevel(int debugLevel);
		void dbg_printBody();
		uint64_t getNotesCount();
        std::vector<HumanizedNote> getHumanizedNotes();
        int process();
		int64_t getStartPos() { return m_mtrkStartPos; }
		int64_t getEndPos() { return m_mtrkEndPos; }
		int64_t getSize() { return m_mtrkSize; }
		std::vector<uint8_t> getFirstDelays() { return m_firstDelays; }

	public: // Public getters
		std::string getTrackText() { return m_trackText; }
		std::string getCopyright() { return m_copyright; }
		std::string getTrackName() { return m_trackName; }
		std::string getInstrumentName() { return m_instrumentName; }
		std::string getVocalText() { return m_vocalText; }
		std::string getTextMarker() { return m_textMarker; }
		std::string getCuePoint() { return m_cuePoint; }
		int8_t getMidiChannel() { return m_midiChannel; }
		int8_t getMidiPort() { return m_midiPort; }
		int32_t getTempo() { return m_tempo; }
		SMPTEOffset getSMPTEOffset() { return m_smpteOffset; }
		TimeSignature getTimeSignature() { return m_timeSignature; }
		KeySignature getKeySignature() { return m_keySignature; }


    private: // Private functions
        int m_work();
		void m_stateMachine();
        void m_debugPrintNoteEvent(NoteEvent note);
        void m_debugPrintHumanizedNote(HumanizedNote hNote);
        void m_mapEventsToHumanizedNotes();
		void m_extractTempo(uint64_t &ptr);
		void m_extractTimeSignature(uint64_t &ptr);
		void m_extractKeySignature(uint64_t &ptr);
		void m_extractStringData(uint64_t &ptr, std::string &outputStr);
		void m_extractSMPTEOffset(uint64_t &ptr);
		void m_parseNoteEvent(uint64_t &ptr, std::vector<NoteEvent> &noteEvents);
        bool m_isTrackEnd(uint64_t startIndex);
        bool m_isNoteEnd(uint64_t startIndex);

    private: // Private data
        STATE m_cms = UNDEFINED; // Current Machine State
        uint8_t m_cmdNoteOnMask = 0x09;		// 0x90
        uint8_t m_cmdNoteOffMask = 0x08;	// 0x80

		uint64_t m_chunkPtr = 0;
        int64_t m_mtrkStartPos = 0;
		int64_t m_mtrkEndPos = 0;
		int64_t m_mtrkSize = 0;

		int m_mtrkOctaveShift = 0;
		int m_mtrkNoteShift = 0;
        int m_debugLevel = 0;

        std::string m_trackText = "";
		std::string m_copyright = "";
		std::string m_trackName = "";
		std::string m_instrumentName = "";
		std::string m_vocalText = "";
		std::string m_textMarker = "";
		std::string m_cuePoint = "";
		uint8_t m_midiChannel = 0xFF;
		uint8_t m_midiPort = 0xFF;
		int32_t m_tempo = 0;
		SMPTEOffset m_smpteOffset;
		TimeSignature m_timeSignature;
		KeySignature m_keySignature;

		std::vector<uint8_t> m_chunkData;
		std::vector<uint8_t> m_firstDelays;
		std::vector<NoteEvent> m_notesOn;
		std::vector<NoteEvent> m_notesOff;
        std::vector<HumanizedNote> m_humanizedNotes;
};

#endif // MTRKCHUNK_H
