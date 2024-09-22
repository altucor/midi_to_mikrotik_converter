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
    SequenceEvent pitch(const uint8_t pitch) const
    {
        return m_pitch.at(pitch);
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
    bool hasNoteOn() const
    {
        auto it = std::find_if(begin(m_pitch), end(m_pitch), [&](const SequenceEvent &event) { return event == SequenceEvent::NOTE_ON; });
        return it != std::end(m_pitch);
    }
    bool hasEvents() const
    {
        auto it = std::find_if(begin(m_pitch), end(m_pitch), [&](const SequenceEvent &event) { return event != SequenceEvent::NONE; });
        return it != std::end(m_pitch);
    }
    uint32_t getTimeDifferenceFromBigger(const TimeMarker &other) const
    {
        return other.time() - time();
    }
    std::size_t eventsCount() const
    {
        std::size_t count = 0;
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

enum class PitchStateValue
{
    NOTE_DISABLED,
    NOTE_ENABLED
};

class PitchState
{
public:
    void updateState(TimeMarker &marker)
    {
        for (uint8_t i = 0; i < kPitchMax; i++)
        {
            if (marker.pitch(i) == SequenceEvent::NOTE_ON)
            {
                m_pitch.at(i) = PitchStateValue::NOTE_ENABLED;
            }
            else if (marker.pitch(i) == SequenceEvent::NOTE_OFF)
            {
                m_pitch.at(i) = PitchStateValue::NOTE_DISABLED;
            }
        }
    }
    std::size_t enabledNotes()
    {
        std::size_t count = 0;
        std::for_each(m_pitch.begin(), m_pitch.end(), [&](const PitchStateValue &val) {
            if (val == PitchStateValue::NOTE_ENABLED)
            {
                count++;
            }
        });
        return count;
    }

private:
    std::array<PitchStateValue, kPitchMax> m_pitch = {PitchStateValue::NOTE_DISABLED};
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
    const std::string prefix() const
    {
        return "[Sequence ch#" + std::to_string(m_channel) + "] ";
    }
    void appenMidiNoteEvent(const bool on, const uint8_t pitch, const uint32_t time)
    {
        BOOST_LOG_TRIVIAL(debug) << prefix() << "event add: " << (on ? " ON" : "OFF") << " pitch: " << std::to_string(pitch) << " time: " << std::to_string(time);
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
    void analyze()
    {
        std::vector<MikrotikTrack> mikrotikTracks;
        mikrotikTracks.push_back(MikrotikTrack());
        BOOST_LOG_TRIVIAL(info) << prefix() << "analysis started, total time markers: " << std::to_string(m_timeMarkers.size());
        PitchState currentPitchState;
        std::for_each(m_timeMarkers.begin(), m_timeMarkers.end(), [&](TimeMarker &marker) {
            /*
             * In general there can be cases like this:
             * 1) TimeMarker have more than 1 event
             * 2) PitchState before update can have already several notes enabled
             * 3) PitchState after update can have several notes enabled
             */
            if (marker.eventsCount() > 1)
            {
                // also do something
            }
            currentPitchState.updateState(marker);
            if (currentPitchState.enabledNotes() > mikrotikTracks.size())
            {
                std::size_t diff = currentPitchState.enabledNotes() - mikrotikTracks.size();
                BOOST_LOG_TRIVIAL(info) << prefix() << "found overlapped notes at: " << std::to_string(marker.time()) << " requires " << std::to_string(diff)
                                        << " additional tracks";
                for (std::size_t i = 0; i < diff; i++)
                {
                    mikrotikTracks.push_back(MikrotikTrack());
                }
            }

            // currentTimeMarker.dumpEvents();
        });
        if (mikrotikTracks.size() > 0 && m_timeMarkers.size() > 0)
        {
            BOOST_LOG_TRIVIAL(info) << prefix() << "total tracks used: " << std::to_string(mikrotikTracks.size());
        }
    }

private:
    uint8_t m_channel = 0;
    std::vector<TimeMarker> m_timeMarkers;
};
