#ifndef MTRKCHUNK_H
#define MTRKCHUNK_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

struct Note
{
    uint8_t channel = 0;
    uint8_t pitch = 0;
    uint8_t velocity = 0;
    uint8_t length = 0;
};

class MTrkChunk
{
    public:
        // Functions
        MTrkChunk();
        virtual ~MTrkChunk();
        int setInitData( const int &chunk_ptr, const std::vector<unsigned char> &chunk_data, const int &_mtrk_octave_shift, const int &_mtrk_note_shift );

        int getStartPos();
        int getEndPos();
        int getSize();
        int getEventsNoteONCount();
        int getEventsNoteOFFCount();
        int getTempo();
        int getTrackNameLength();
        std::string getTrackName();
        int getTrackTextLength();
        std::string getTrackText();
        int getNoteChannel();

        std::vector<double> getFreqNotes();
        std::vector<std::vector<int>> getLengthNotesON();
        std::vector<std::vector<int>> getLengthNotesOFF();
        std::vector<std::string> getSymbolicNotes();
        //vector<int> getFirstDelay();

        void dbg_printBody();
        void dbg_printSymbolicNotes();
        void dbg_printFreqNotes();

        // Variables

    private:
        // Functions
        int m_work();
        int m_detect_events_note_on();
        int m_detect_events_note_off();
        int m_detectTempo();
        std::string m_detectTrackName();
        void m_detect_text();
        int m_found_note_channel();
        //void m_detect_first_delay();

        // Variables
        int m_cmdNoteOn  = 0x90; // 0x90
        int m_cmdNoteOff =  0x80; //0x80
        int m_chunkPtr = 0;
        std::vector<unsigned char> m_chunkData;
        int m_mtrkOctaveShift = 0;
        int m_mtrkNoteShift = 0;
        int m_mtrkStartPos = 0;
        int m_mtrkEndPos = 0;
        int m_mtrkSize = 0;
        int m_tempo = 0;
        uint64_t m_trackNameLength = 0;
        std::string m_trackName = "";
        int m_trackTextLength = 0;
        std::string m_trackText = "";
        int m_noteChannel = 0;
        int m_firstNoteOnByteAddr = 0;
        std::vector<unsigned char> m_chunkBody;
        std::vector<std::vector<unsigned char>> m_eventsNoteOn;
        std::vector<std::vector<unsigned char>> m_eventsNoteOff;
        std::vector<double> m_chunkFreqNotes;
        std::vector<std::vector<int>> m_chunkLengthNotesOn;
        std::vector<std::vector<int>> m_chunkLengthNotesOff;
        std::vector<std::string> m_chunkSymbolicNotes;
        std::vector<int> m_firstDelay;
};

#endif // MTRKCHUNK_H
