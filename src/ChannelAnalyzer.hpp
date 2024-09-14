#pragma once

#include "util.h"

#include "Config.hpp"
#include "MikrotikTrack.hpp"
#include "Sequence.hpp"

#include "boost/log/trivial.hpp"

#include <algorithm>
#include <string>
#include <vector>

class ChannelAnalyzer
{
public:
    ChannelAnalyzer()
    {
    }
    ChannelAnalyzer(Config &config, const std::size_t trackIndex, const uint8_t channel) : m_config(config), m_trackIndex(trackIndex), m_channel(channel)
    {
        m_pps = pulses_per_second(config.ppqn, config.bpm);
        m_tracks.push_back(MikrotikTrack(config, m_channel));
        m_sequence = Sequence(m_channel);
    }
    void setTrackInfo(TrackMetaTextInfo &info)
    {
        m_sequence.analyze();
        /*
         * later_bitches.mid
         * later_bitches-3xOsc-1-0.txt
         */
        std::string fileName = m_config.inFileName + "-" + info.track + "-" + std::to_string(m_trackIndex) + "-" + std::to_string(m_channel);
        for (std::size_t i = 0; i < m_tracks.size(); i++)
        {
            m_tracks.at(i).setTrackInfo(info);
            if (i > 0)
            {
                // overlaped notes track
                fileName += "-overlaped-" + std::to_string(i);
            }
            m_tracks.at(i).setFileName(fileName + ".txt");
        }
    }
    std::vector<MikrotikTrack> getTracks()
    {
        return m_tracks;
    }
    const uint8_t channel() const
    {
        return m_channel;
    }
    void addEvent(midi_event_smf_t event)
    {
        BOOST_LOG_TRIVIAL(debug) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] " << "got SMF event: status: " << std::to_string(event.message.status)
                                 << " subCmd: " << std::to_string(event.message.subCmd);

        if (!m_noteOn.on)
        {
            m_currentPredelay += event.predelay.val;
        }

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

        m_sequence.appenMidiNoteEvent(event.event.note.on, event.event.note.pitch, m_channelDuration);

        if (event.event.note.on)
        {
            if (m_noteOn.pitch != 0)
            {
                // m_currentTrackIndex++;
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

            float noteDuration = duration_to_ms(m_durationAccumulator, m_pps);
            float preDelay = duration_to_ms(m_currentPredelay, m_pps);
            // BOOST_LOG_TRIVIAL(info) << "[ChannelAnalyzer #" << std::to_string(m_channel) << " ] " << "found note: pitch: " << std::to_string(m_noteOn.pitch)
            //                         << " duration: " << std::to_string(noteDuration);
            m_tracks[m_currentTrackIndex].append(MikrotikNote(m_noteOn.pitch, noteDuration, preDelay));
            m_durationAccumulator = 0;
            m_currentPredelay = 0;
            m_noteOn = {0};
            return;
        }
    }

private:
    Config m_config;
    std::size_t m_trackIndex = 0;
    uint8_t m_channel = 0;
    float m_pps = 0.0f;
    bool m_firstEventSkipDelay = false;
    uint64_t m_currentPredelay = 0;
    uint64_t m_durationAccumulator = 0;
    uint64_t m_channelDuration = 0;
    midi_note_t m_noteOn = {0};
    std::size_t m_currentTrackIndex = 0;
    std::vector<MikrotikTrack> m_tracks;
    Sequence m_sequence = {};
};