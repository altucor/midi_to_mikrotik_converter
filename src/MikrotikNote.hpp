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
    MikrotikNote(const uint8_t pitch, const uint32_t duration, const uint32_t predelay) : m_pitch(pitch), m_duration(duration), m_predelay(predelay)
    {
    }
    void log() const
    {
        BOOST_LOG_TRIVIAL(info) << "Note " << " pitch: " << (uint32_t)m_pitch << " duration ticks: " << (uint32_t)m_duration;
    }
    uint8_t pitch() const
    {
        return m_pitch;
    }
    uint32_t duration() const
    {
        return m_duration;
    }
    uint32_t preDelay() const
    {
        return m_predelay;
    }
    std::string beepLine(const PitchShift &pitch) const
    {
        /*
         * :beep frequency=440 length=1000ms; # C4 + 35Hz @ HH:MM:SS:MS
         * :delay 1000ms;
         */

        std::stringstream ss;
        ss << ":beep frequency=" << std::to_string(pitch.freq(m_pitch));
        ss << " length=" << m_duration << "ms;";

        return ss.str();
    }
    std::string preDelayLine()
    {
        return Utils::getDelayLine(preDelay());
    }
    std::string delayLine()
    {
        return Utils::getDelayLine(duration());
    }

private:
    uint8_t m_pitch = 0;
    uint32_t m_duration = 0;
    uint32_t m_predelay = 0;
};
