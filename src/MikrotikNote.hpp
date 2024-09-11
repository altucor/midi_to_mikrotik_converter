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
    MikrotikNote(Config &config, const uint8_t pitch, const uint32_t duration, const uint32_t predelay, const uint32_t timeMarker)
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

        uint8_t shiftedPitch = m_config.pitchShift.process(m_pitch);
        float freq = pitch_to_freq(shiftedPitch) + m_config.pitchShift.fine;

        std::stringstream ss;
        ss << ":beep frequency=" << std::to_string(freq);
        ss << " length=" << m_duration << "ms;";

        if (m_config.comments)
        {
            ss << " # " << std::string(pitch_to_name(shiftedPitch));
            if (m_config.pitchShift.fine != 0.0f)
            {
                ss << std::string(" ");
                ss << ((m_config.pitchShift.fine < 0.0f) ? "-" : "+");
                ss << m_config.pitchShift.fine;
                ss << std::string(" Hz");
            }
            ss << " @ " << Utils::getTimeAsText(m_timeMarker);
        }
        ss << "\n";

        return ss.str();
    }
    std::string toStringWithDelay() const
    {
        std::string out;
        if (m_predelay != 0)
        {
            out += Utils::getDelayLine(m_predelay);
        }
        out += toString();
        out += Utils::getDelayLine(m_duration) + "\n";
        return out;
    }
    void toStringWithDelay(std::ofstream &out)
    {
        out << toStringWithDelay();
    }
    uint32_t duration() const
    {
        return m_duration;
    }

private:
    Config m_config;
    uint8_t m_pitch = 0;
    uint32_t m_duration = 0;
    uint32_t m_predelay = 0;
    uint32_t m_timeMarker = 0;
};
