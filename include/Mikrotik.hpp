#ifndef MIKROTIK_HPP
#define MIKROTIK_HPP

#include "Note.hpp"
#include "mtrk.h"
#include <vector>

class MikrotikConfig
{
public:
    bool comments = false;
    uint16_t ppqn;
    int32_t bpm;
    uint64_t track_index = 0;
    PitchShift pitchShift;
    std::string inFileName = "";
    std::string outFileName = "";
};

class Mikrotik
{
public:
    Mikrotik(MikrotikConfig &config, const std::size_t index, mtrk_t *track) : m_config(config), m_track(track), m_index(index)
    {
    }
    int buildScriptForChannel(std::string &fileName, const uint8_t channel);
    int buildScript();

private:
    std::string getTimeAsText(const double time);
    std::string getTrackTimeLength(const uint8_t channel);
    std::string getNotesCount(const uint8_t channel);
    void getScriptHeader(std::stringstream &out, const uint8_t channel);

private:
    MikrotikConfig m_config = {0};
    std::size_t m_index = 0;
    mtrk_t *m_track;
    double m_processedTime = 0.0;
};

#endif // MIKROTIK_HPP
