#pragma once

#include "mtrk.h"

#include "NoteTrack.hpp"

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

class MidiTrackChannelEvents
{
    uint32_t channel[MIDI_CHANNELS_MAX_COUNT] = {0};
};

class MikrotikTrack
{
public:
    MikrotikTrack(MikrotikConfig &config, const std::size_t index, mtrk_t *track) : m_config(config), m_track(track), m_index(index)
    {
        m_pps = pulses_per_second(m_config.ppqn, m_config.bpm);
        for (uint8_t i = 0; i < MIDI_CHANNELS_MAX_COUNT; i++)
        {
            m_noteTracks.push_back(NoteTrack(i));
        }
    }
    int analyze();
    int buildScriptForChannel(const uint8_t channel);
    int buildScript();

private:
    std::string getTimeAsText(const double time);
    std::string getTrackTimeLength(const uint8_t channel);
    std::string getNotesCount(const uint8_t channel);
    void getScriptHeader(std::ofstream &out, const uint8_t channel);

private:
    MikrotikConfig m_config = {0};
    float m_pps = 0.0f;
    std::size_t m_channelEvents[MIDI_CHANNELS_MAX_COUNT] = {0};
    std::size_t m_index = 0;
    mtrk_t *m_track;
    double m_processedTime = 0.0;
    std::vector<NoteTrack> m_noteTracks;
};
