#pragma once

#include "util.h"

#include "Config.hpp"
#include "Utils.hpp"

#include <boost/log/trivial.hpp>

#include <fstream>
#include <sstream>

bool isInRange(const uint32_t val, const uint32_t min, const uint32_t max)
{
    return min < val && val < max;
}

class MikrotikNote
{
public:
    MikrotikNote()
    {
    }
    MikrotikNote(const uint8_t pitch, const uint32_t start, const uint32_t end) : m_pitch(pitch), m_timeStart(start), m_timeEnd(end)
    {
    }
    const std::string prefix() const
    {
        return "[Note % " + std::to_string(m_pitch) + "] ";
    }
    void log() const
    {
        BOOST_LOG_TRIVIAL(debug) << prefix() << " start: " << (uint32_t)m_timeStart << " end: " << (uint32_t)m_timeEnd;
    }
    uint8_t pitch() const
    {
        return m_pitch;
    }
    uint32_t start() const
    {
        return m_timeStart;
    }
    uint32_t end() const
    {
        return m_timeEnd;
    }
    uint32_t duration() const
    {
        return m_timeEnd - m_timeStart;
    }
    uint32_t getPredelayFromPrevious(const MikrotikNote &other) const
    {
        return m_timeStart - other.m_timeEnd;
    }
    bool intersectsWith(const MikrotikNote &other) const
    {
        /*
         * 1) 10-20 and 30-40
         * 2) 10-20 and 15-25
         * 3) 10-20 and 05-15
         * 4) 10-20 and 20-30
         *
         */

        // if (m_timeStart == other.m_timeStart || m_timeEnd == other.m_timeEnd)
        if (m_timeStart == other.m_timeStart)
        {
            BOOST_LOG_TRIVIAL(debug) << prefix() << " case 1.1";
            return true;
        }

        if (m_timeEnd == other.m_timeEnd)
        {
            BOOST_LOG_TRIVIAL(debug) << " ------------------------ ";
            BOOST_LOG_TRIVIAL(debug) << prefix() << " case 1.2";
            BOOST_LOG_TRIVIAL(debug) << " Original note:";
            log();
            BOOST_LOG_TRIVIAL(debug) << " Other note:";
            other.log();
            BOOST_LOG_TRIVIAL(debug) << " ------------------------ ";
            return true;
        }

        if (m_timeStart < other.m_timeStart && isInRange(m_timeEnd, other.m_timeStart, m_timeEnd))
        {
            BOOST_LOG_TRIVIAL(debug) << prefix() << " case 2";
            return true;
        }
        else if (other.m_timeStart < m_timeStart && isInRange(other.m_timeEnd, m_timeStart, m_timeEnd))
        {
            BOOST_LOG_TRIVIAL(debug) << prefix() << " case 3";
            return true;
        }

        return false;
    }
    std::string beepLine(const PitchShift &pitch, const float pps) const
    {
        /*
         * :beep frequency=440 length=1000ms; # C4 + 35Hz @ HH:MM:SS:MS
         * :delay 1000ms;
         */

        std::stringstream ss;
        ss << ":beep frequency=" << std::to_string(pitch.freq(m_pitch));
        ss << " length=" << duration_to_ms(duration(), pps) << "ms;";

        return ss.str();
    }

private:
    uint8_t m_pitch = 0;
    uint32_t m_timeStart = 0;
    uint32_t m_timeEnd = 0;
};
