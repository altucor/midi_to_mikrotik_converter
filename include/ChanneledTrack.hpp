#pragma once

#include "mtrk.h"

#include "MikrotikNote.hpp"

#include <vector>

class ChanneledTrack
{
public:
    ChanneledTrack(mtrk_t *track, const uint8_t channel = 0) : m_track(track), m_channel(channel)
    {
    }
    void analyze()
    {
        uint32_t eventsCount = mtrk_get_events_count(m_track);

        for (uint32_t i = 0; i < eventsCount; i++)
        {
            midi_event_smf_t *event = mtrk_get_event(m_track, i);
            if (event->message.status != MIDI_STATUS_SYSTEM)
            {
            }
        }
    }

private:
    mtrk_t *m_track = nullptr;
    uint8_t m_channel = 0;
    std::vector<MikrotikNote> m_notes;
};