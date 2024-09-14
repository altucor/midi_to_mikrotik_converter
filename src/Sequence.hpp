#pragma once

#include "events/note.h"

#include <boost/log/trivial.hpp>

#include <algorithm>
#include <array>
#include <sstream>
#include <vector>

constexpr uint8_t kPitchMax = 128;

enum class SequenceEvent
{
    NONE = 0,
    NOTE_ON,
    NOTE_OFF,
};

class TimeMarker
{
public:
    TimeMarker(const uint32_t time) : m_timeMarker(time)
    {
    }
    uint32_t time() const
    {
        return m_timeMarker;
    }
    void dumpEvents() const
    {
        std::stringstream ss;
        for (uint8_t i = 0; i < kPitchMax; i++)
        {
            if (m_pitch.at(i) == SequenceEvent::NOTE_ON || m_pitch.at(i) == SequenceEvent::NOTE_OFF)
            {
                ss << "pitch: " << std::to_string(i) << " ";
                ss << std::to_string(static_cast<uint8_t>(m_pitch.at(i))) << " ";
            }
        }
        BOOST_LOG_TRIVIAL(info) << "[TimeMarker] enabled events: " << ss.str();
    }
    bool hasNoteOffAt(const uint8_t pitch) const
    {
        return m_pitch.at(pitch) == SequenceEvent::NOTE_OFF;
    }
    bool hasNoteOn() const
    {
        auto it = std::find_if(begin(m_pitch), end(m_pitch), [&](const SequenceEvent &event) { return event == SequenceEvent::NOTE_ON; });
        return it != std::end(m_pitch);
    }
    uint32_t getTimeDifferenceFromBigger(const TimeMarker &other) const
    {
        return other.time() - time();
    }
    bool overlapsWith(const TimeMarker &other) const
    {
        return hasNoteOn() && other.hasNoteOn();
    }
    void updateState(const TimeMarker &other)
    {
        for (uint8_t i = 0; i < kPitchMax; i++)
        {
            if (other.m_pitch.at(i) != SequenceEvent::NONE)
            {
                // if (m_pitch.at(i) == SequenceEvent::NONE)
                // {
                m_pitch.at(i) = other.m_pitch.at(i);
                // }
            }
        }
    }
    bool hasMultipleEvents() const
    {
        uint8_t count = 0;
        std::for_each(m_pitch.begin(), m_pitch.end(), [&](const SequenceEvent &event) {
            if (event == SequenceEvent::NOTE_ON || event == SequenceEvent::NOTE_OFF)
            {
                count++;
            }
        });
        return count;
    }
    bool timeEquals(const uint32_t otherTime) const
    {
        return otherTime == m_timeMarker;
    }
    void setNote(const bool on, const uint8_t pitch)
    {
        m_pitch.at(pitch) = on ? SequenceEvent::NOTE_ON : SequenceEvent::NOTE_OFF;
    }

private:
    std::array<SequenceEvent, kPitchMax> m_pitch = {SequenceEvent::NONE};
    uint32_t m_timeMarker = 0;
};

class Sequence
{
public:
    Sequence()
    {
    }
    Sequence(const uint8_t channel) : m_channel(channel)
    {
    }
    void appenMidiNoteEvent(const bool on, const uint8_t pitch, const uint32_t time)
    {
        BOOST_LOG_TRIVIAL(info) << "[Sequence ch#" << std::to_string(m_channel) << "] event add: " << (on ? " ON" : "OFF") << " pitch: " << std::to_string(pitch)
                                << " time: " << std::to_string(time);
        if (auto it = std::find_if(begin(m_timeMarkers), end(m_timeMarkers), [&](TimeMarker &marker) { return marker.timeEquals(time); });
            it != std::end(m_timeMarkers))
        {
            it->setNote(on, pitch);
        }
        else
        {
            m_timeMarkers.push_back(TimeMarker(time));
            m_timeMarkers.back().setNote(on, pitch);
        }
    }
    uint32_t findNoteOffEventFor(const uint8_t pitch, const uint32_t startTime = 0) const
    {
        if (auto it = std::find_if(begin(m_timeMarkers), end(m_timeMarkers),
                                   [&](const TimeMarker &marker) { return marker.time() >= startTime && marker.hasNoteOffAt(pitch); });
            it != std::end(m_timeMarkers))
        {
            return it->time();
        }
        return 0;
    }
    void analyze()
    {
        BOOST_LOG_TRIVIAL(info) << "[Sequence ch#" << std::to_string(m_channel) << "] analysis started";
        TimeMarker currentTimeMarker(0);
        std::for_each(m_timeMarkers.begin(), m_timeMarkers.end(), [&](TimeMarker &marker) {
            if (currentTimeMarker.overlapsWith(marker))
            {
                BOOST_LOG_TRIVIAL(info) << "[Sequence ch#" << std::to_string(m_channel) << "] found overlapped notes at: " << std::to_string(marker.time());
            }
            currentTimeMarker.updateState(marker);
            // currentTimeMarker.dumpEvents();
        });
    }

private:
    uint8_t m_channel = 0;
    std::vector<TimeMarker> m_timeMarkers;
};
