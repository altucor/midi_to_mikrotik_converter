#pragma once

#include "mtrk.h"

#include "ChannelAnalyzer.hpp"

#include "boost/log/trivial.hpp"

#include <vector>

class TrackAnalyzer
{
public:
    TrackAnalyzer(mtrk_t *track, const float pps) : m_track(track), m_pps(pps)
    {
        for (uint8_t i = 0; i < MIDI_CHANNELS_MAX_COUNT; i++)
        {
            m_channels[i] = ChannelAnalyzer(i, m_pps);
        }
    }
    void analyze()
    {
        uint32_t eventsCount = mtrk_get_events_count(m_track);
        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyze] available midi events: " << std::to_string(eventsCount);
        for (uint32_t i = 0; i < eventsCount; i++)
        {
            midi_event_smf_t *event = mtrk_get_event(m_track, i);
            m_trackDuration += event->predelay.val;
            if (event->message.status == MIDI_STATUS_NOTE_OFF || event->message.status == MIDI_STATUS_NOTE_ON)
            {
                for (uint8_t i = 0; i < MIDI_CHANNELS_MAX_COUNT; i++)
                {
                    m_channels.at(i).addEvent(*event);
                }
            }
            else if (event->message.status == MIDI_STATUS_SYSTEM)
            {
                BOOST_LOG_TRIVIAL(info) << "[TrackAnalyze] found system event: " << std::to_string(event->message_meta);
                // store additional events
            }
        }
    }
    ChannelAnalyzer &getChannel(const uint8_t channel)
    {
        return m_channels.at(channel);
    }

private:
    mtrk_t *m_track = nullptr;
    const float m_pps = 0.0f;
    uint64_t m_trackDuration = 0;
    std::array<ChannelAnalyzer, MIDI_CHANNELS_MAX_COUNT> m_channels;
};
