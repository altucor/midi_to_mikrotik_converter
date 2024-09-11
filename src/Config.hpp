#pragma once

#include "boost/log/trivial.hpp"

#include <fstream>
#include <string>

class PitchShift
{
public:
    int8_t octave = 0;
    int8_t note = 0;
    float fine = 0.0f;
    uint8_t process(const uint8_t pitch) const
    {
        return pitch + (octave * MIDI_NOTES_IN_OCTAVE) + note;
    }
    void toStream(std::ofstream &out)
    {
        out << "octave: " << (octave >= 0 ? "+" : "-") << std::to_string(octave);
        out << " note: " << (note >= 0 ? "+" : "-") << std::to_string(note);
        out << " fine: " << (fine >= 0.0f ? "+" : "-") << std::to_string(fine);
    }
};

class Config
{
public:
    bool comments = false;
    boost::log::trivial::severity_level logLevel = boost::log::trivial::severity_level::info;
    uint16_t ppqn = 0;
    int32_t bpm = 0;
    uint64_t track_index = 0;
    PitchShift pitchShift = {0};
    std::string inFileName = "";
    std::string outFileName = "";
};
