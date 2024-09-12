#pragma once

#include "util.h"

#include "Config.hpp"
#include "Utils.hpp"

#include <boost/log/trivial.hpp>

#include <fstream>
#include <sstream>

class MikrotikNote
{
public:
    MikrotikNote(PitchShift &config, const uint8_t pitch, const uint32_t duration, const uint32_t predelay, const uint32_t timeMarker)
        : m_config(config), m_pitch(pitch), m_duration(duration), m_predelay(predelay), m_timeMarker(timeMarker)
    {
    }
    void log() const
    {
        BOOST_LOG_TRIVIAL(info) << "Note " << " pitch: " << (uint32_t)m_pitch << " duration ticks: " << (uint32_t)m_duration;
    }
    std::string toString() const
    {
        /*
         * :beep frequency=440 length=1000ms; # C4 + 35Hz @ HH:MM:SS:MS
         * :delay 1000ms;
         */

        uint8_t shiftedPitch = m_config.process(m_pitch);
        float freq = pitch_to_freq(shiftedPitch) + m_config.fine;

        std::stringstream ss;
        ss << ":beep frequency=" << std::to_string(freq);
        ss << " length=" << m_duration << "ms;";

        return ss.str();
    }
    uint8_t pitch() const
    {
        return m_pitch;
    }
    uint8_t pitchShifted() const
    {
        return m_config.process(m_pitch);
    }
    uint32_t duration() const
    {
        return m_duration;
    }
    uint32_t preDelay() const
    {
        return m_predelay;
    }

private:
    PitchShift m_config;
    uint8_t m_pitch = 0;
    uint32_t m_duration = 0;
    uint32_t m_predelay = 0;
    uint32_t m_timeMarker = 0;
};
