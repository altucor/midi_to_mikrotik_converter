#pragma once

#include "events/note.h"

#include "MikrotikNote.hpp"

#include <boost/log/trivial.hpp>

#include <algorithm>
#include <array>
#include <sstream>
#include <vector>

const uint8_t kPitchMax = 128;

enum class SequenceEvent
{
    NONE = 0,
    NOTE_ON,
    NOTE_OFF,
    NOTE_OFF_ON,
};

class TimeMarker
{
public:
    TimeMarker(const uint32_t time, const bool on, const uint8_t pitch) : m_timeMarker(time)
    {
        setNote(on, pitch);
    }
    void setNote(const bool on, const uint8_t pitch)
    {
        if (m_pitch.at(pitch) == SequenceEvent::NOTE_OFF && on)
        {
            m_pitch.at(pitch) = SequenceEvent::NOTE_OFF_ON;
            return;
        }
        m_pitch.at(pitch) = on ? SequenceEvent::NOTE_ON : SequenceEvent::NOTE_OFF;
    }
    const std::string prefix() const
    {
        return "[TimeMarker @ " + std::to_string(m_timeMarker) + "] ";
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
        BOOST_LOG_TRIVIAL(info) << prefix() << "enabled events: " << ss.str();
    }
    bool hasNoteOffAtPitch(const uint32_t pitch) const
    {
        return m_pitch.at(pitch) == SequenceEvent::NOTE_OFF || m_pitch.at(pitch) == SequenceEvent::NOTE_OFF_ON;
    }
    bool timeEquals(const uint32_t otherTime) const
    {
        return otherTime == m_timeMarker;
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
    Sequence(const std::size_t trackIndex, const uint8_t channel) : m_trackIndex(trackIndex), m_channel(channel)
    {
    }
    const std::string prefix() const
    {
        return "[Sequence ch# " + std::to_string(m_channel) + "] ";
    }
    void setTrackInfo(TrackMetaTextInfo &info)
    {
        m_metaTextInfo = info;
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
            m_timeMarkers.push_back(TimeMarker(time, on, pitch));
        }
    }
    MikrotikNote getNote(const uint32_t startTime, const uint8_t pitch) const
    {
        MikrotikNote note;

        auto it = std::find_if(begin(m_timeMarkers), end(m_timeMarkers),
                               [&](const TimeMarker &marker) { return marker.time() > startTime && marker.hasNoteOffAtPitch(pitch); });

        if (it == std::end(m_timeMarkers))
        {
            return note;
        }

        note = MikrotikNote(pitch, startTime, it->time());
        return note;
    }
    std::vector<MikrotikTrack> analyze()
    {
        std::size_t trackSequenceIndex = 0;
        std::vector<MikrotikTrack> mikrotikTracks;
        mikrotikTracks.push_back(MikrotikTrack(m_trackIndex, m_channel, trackSequenceIndex++));
        mikrotikTracks.back().setTrackInfo(m_metaTextInfo);
        if (m_timeMarkers.size() == 0)
        {
            return mikrotikTracks;
        }
        BOOST_LOG_TRIVIAL(info) << prefix() << "analysis started, total time markers: " << std::to_string(m_timeMarkers.size());
        std::for_each(m_timeMarkers.begin(), m_timeMarkers.end(), [&](TimeMarker &marker) {
            for (uint8_t i = 0; i < kPitchMax; i++)
            {
                if (marker.pitch(i) == SequenceEvent::NOTE_OFF || marker.pitch(i) == SequenceEvent::NONE)
                {
                    continue;
                }
                auto note = getNote(marker.time(), i);
                note.log();

                if (std::ranges::none_of(mikrotikTracks, [&](MikrotikTrack &track) { return track.tryAppend(note); }))
                {
                    BOOST_LOG_TRIVIAL(info) << prefix() << "No free Track, creating new and adding to it";
                    mikrotikTracks.push_back(MikrotikTrack(m_trackIndex, m_channel, trackSequenceIndex++));
                    mikrotikTracks.back().setTrackInfo(m_metaTextInfo);
                    mikrotikTracks.back().tryAppend(note);
                }
            }
        });
        if (mikrotikTracks.size() > 0 && m_timeMarkers.size() > 0)
        {
            BOOST_LOG_TRIVIAL(info) << prefix() << "total tracks used: " << std::to_string(mikrotikTracks.size());
        }

        return mikrotikTracks;
    }

private:
    std::size_t m_trackIndex = 0;
    uint8_t m_channel = 0;
    TrackMetaTextInfo m_metaTextInfo;
    std::vector<TimeMarker> m_timeMarkers;
};
