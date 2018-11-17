#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include "MTrkChunk.h"

struct MidiMthdHeader
{
    char mthd[4] = {0};
    uint32_t mthdChunkLength = 0;
    uint16_t formatType = 0;
    uint16_t mtrkChunksCount = 0;
    uint16_t ppqn = 0;
};

struct MtrkChunkInfo
{
    char mtrk[4] = {0};
    uint64_t startPos = 0;
    uint64_t endPos = 0;
    uint64_t size = 0;
    MTrkChunk *mtrkChunkHandler;
};

class MidiFile
{
    public:
        // Persists

        // Functions
        MidiFile();
        ~MidiFile();
        void setFilePath(std::string &filePath);
        void setOctaveShift(int octaveShift);
        void setNoteShift(int noteShift);
        void setNewBpm(int bpm);
        void setEnableCommentsTo(bool enableCommentsFlag);
        void parseFile();
        std::string getPath();
        int getFileSize();
        std::vector<uint8_t> getFileData();
        double getFullTrackLength();

    private:
        // Persists
        std::string m_filePath;
        int m_octaveShift = 0;
        int m_noteShift = 0;
        int m_newBpm = -1;
        bool m_commentsFlag = false;
        uint64_t m_fileSize = 0;
        int m_filePtr = 0; //file pointer
        std::vector<uint8_t> m_fileData;
        //vector<struct> mtrk_chunks_arr;
        //int mtrk_chunk_length = 0;
        bool m_fileSuccessReaded = false;
        int m_mthdChunkLength = 0;
        int format_type = 0;
        int m_mtrkChunksCnt = 0;
        int m_ppqn = 0;
        int m_tempoTrack = 0;
        double pulses_per_sec = 0.0;
        double full_track_length = 0.0; // in seconds (like timestamp)
        int ftl_h = 0; //ftl - full_track_length in hours
        int ftl_m = 0; //ftl - full_track_length in minutes
        int ftl_s = 0; //ftl - full_track_length in secondss
        std::vector<double> freq_notes_arr;
        std::vector<std::vector<int>> length_notes_on_arr;
        std::vector<std::vector<int>> length_notes_off_arr;
        std::vector<std::string> symbolic_notes_arr;
        std::vector<int> first_dly;
        std::vector<MtrkChunkInfo> m_mtrkChunks;

        // Functions
        int m_readFile( std::string path);
        int m_parseHeader();
        int m_findMtrkChunks();
        int m_processChunk(MtrkChunkInfo chunkInfo);
        int m_findChunksAndDecode();
        int m_createMikrotikScriptFile( const int &chunk_number, const std::string &track_text, const std::string &track_name_comment, const int &notes_cnt, const int &note_ch );
        void m_humanizeTime();
};

#endif // MIDIFILE_H
