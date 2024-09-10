#pragma once

#include "MikrotikNote.hpp"

#include "boost/log/trivial.hpp"

#include <string>
#include <vector>

class ChannelAnalyzer
{
public:
    ChannelAnalyzer()
    {
    }
    ChannelAnalyzer(const uint8_t channel, const float pps) : m_channel(channel), m_pps(pps)
    {
    }
    const uint8_t channel() const
    {
        return m_channel;
    }
    const std::size_t notesCount() const
    {
        return m_notes.size();
    }
    const uint64_t duration() const
    {
        return m_channelDuration;
    }
    void toScript(std::ofstream &out)
    {
        std::for_each(m_notes.begin(), m_notes.end(), [&](MikrotikNote &note) { note.toStringWithDelay(out); });
    }
    void addEvent(midi_event_smf_t event)
    {
        BOOST_LOG_TRIVIAL(debug) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] " << "got SMF event: status: " << std::to_string(event.message.status)
                                 << " subCmd: " << std::to_string(event.message.subCmd);
        if (!m_firstEventSkipDelay)
        {
            m_firstEventSkipDelay = true;
        }
        else
        {
            // ignore first event pre-delay
            m_durationAccumulator += event.predelay.val;
            m_channelDuration += event.predelay.val;
        }

        if (event.event.note.channel != m_channel)
        {
            BOOST_LOG_TRIVIAL(debug) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] "
                                     << "ignore event cahnnel:" << std::to_string(event.event.note.channel);
            return;
        }

        // if (m_notes.size() == 0)
        // {
        //     if (event.message.status == MIDI_STATUS_NOTE_OFF)
        //     {
        //         BOOST_LOG_TRIVIAL(debug) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] " << "ignore event first note off";
        //         return;
        //     }
        // }

        if (event.event.note.on)
        {
            if (m_noteOn.pitch != 0)
            {
                BOOST_LOG_TRIVIAL(debug) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] " << "ignore event overlay, double on";
                return;
            }
            m_durationAccumulator = 0;
            m_noteOn = event.event.note;
            return;
        }
        else
        {
            if (m_noteOn.pitch != event.event.note.pitch)
            {
                BOOST_LOG_TRIVIAL(debug) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] " << "ignore event overlay, invalid pitch";
                return;
            }

            float msDuration = duration_to_ms(m_durationAccumulator, m_pps);
            BOOST_LOG_TRIVIAL(info) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] " << "found note: pitch: " << std::to_string(m_noteOn.pitch)
                                    << " duration: " << std::to_string(msDuration);
            m_notes.push_back(MikrotikNote(m_noteOn.pitch, msDuration));
            m_durationAccumulator = 0;
            m_noteOn = {0};
            return;
        }
    }

private:
    float m_pps = 0.0f;
    uint8_t m_channel = 0;
    bool m_firstEventSkipDelay = false;
    uint64_t m_durationAccumulator = 0;
    uint64_t m_channelDuration = 0;
    midi_note_t m_noteOn = {0};
    std::vector<MikrotikNote> m_notes;
    std::vector<MikrotikNote> m_notesOverlayed;
};