#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>
#include <iomanip>
#include <stdlib.h>
#include "MTrkChunk.h"

struct TrackLength
{
    uint32_t h = 0;
    uint32_t m = 0;
    uint32_t s = 0;
    double ms = 0;
};

struct MidiMthdHeader
{
    char mthd[5] = {0,0,0,0,'\0'};
    uint32_t mthdChunkLength = 0;
    uint16_t formatType = 0;
    uint16_t mtrkChunksCount = 0;
    uint16_t ppqn = 0;
};

struct MtrkChunkInfo
{
    char mtrk[5] = {0,0,0,0,'\0'};
    uint64_t startPos = 0;
    uint64_t endPos = 0;
    uint64_t size = 0;
    uint32_t chunkNumber = 0;
    MTrkChunk mtrkChunkHandler;
};

class MidiFile
{
    public:
        // Functions
        explicit MidiFile();
        ~MidiFile();
        void setFilePath(std::string &filePath);
        void setOctaveShift(int octaveShift);
        void setNoteShift(int noteShift);
        void setNewBpm(int bpm);
        void setDebugLevel(int debugLevel);
        void setEnableCommentsTo(bool enableCommentsFlag);
        void setPredelayFlag(bool predelayFlag);
        void parseFile();
        std::string getPath();
		int64_t getFileSize();
        std::vector<uint8_t> getFileData();

    private:
        // Variables
        int m_octaveShift = 0;
        int m_noteShift = 0;
        int m_debugLevel = 0;
        int m_newBpm = -1;
        bool m_commentsFlag = false;
        bool m_predelayFlag = false;
        uint64_t m_fileSize = 0;
		int64_t m_filePtr = 0; // file pointer
        int m_mthdChunkLength = 0;
        int m_formatType = 0;
        int m_mtrkChunksCnt = 0;
        int m_ppqn = 0;
        int m_tempoTrack = 0;
        double m_pulsesPerSec = 0.0;
        std::string m_filePath;
        std::vector<uint8_t> m_fileData;
        std::string m_trackText = "";
        std::string m_trackName = "";
        std::vector<MtrkChunkInfo> m_mtrkChunks;

    private:
        // Functions
        int m_readFile();
        int m_parseHeader();
        int m_findMtrkChunks();
        int m_processChunks();
        int m_processChunk(MtrkChunkInfo &chunkInfo);
        int m_generateOutputTrackFiles();
		double m_durationArrayToMiliseconds(std::vector<uint8_t> delayEvents);
        int m_createMikrotikScriptFile(MtrkChunkInfo chunk);
        double m_calculateFullTrackSize(MtrkChunkInfo & chunkInfo);
        TrackLength m_converTimeToReadable(double lengthInMs);
        double m_calculateFirstDelay(std::vector<uint8_t> firstDelays);
};

#endif // MIDIFILE_H
