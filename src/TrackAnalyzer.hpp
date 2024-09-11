#pragma once

#include "mtrk.h"

#include "ChannelAnalyzer.hpp"

#include "boost/log/trivial.hpp"

#include <array>
#include <vector>

class TrackMetaTextInfo
{
public:
    std::string text;
    std::string copyright;
    std::string track;
    std::string instrument;
    std::string lyric;
    std::string marker;
    std::string cue;
};

class TrackAnalyzer
{
public:
    TrackAnalyzer(Config &config, mtrk_t *track, const float pps) : m_config(config), m_track(track), m_pps(pps)
    {
        for (uint8_t i = 0; i < MIDI_CHANNELS_MAX_COUNT; i++)
        {
            m_channels[i] = ChannelAnalyzer(m_config, i, m_pps);
        }
    }
    void analyze()
    {
        uint32_t eventsCount = mtrk_get_events_count(m_track);
        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] available midi events: " << std::to_string(eventsCount);
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
                // store additional events
                if (is_meta_text_event(event->message_meta))
                {
                    BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] found TEXT event: " << Utils::toHex(event->message_meta, 1);
                    switch (event->message_meta)
                    {
                    case MIDI_META_EVENT_TEXT:
                        m_metaTextInfo.text = std::string(event->event.meta.text.val);
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] text: " << m_metaTextInfo.text;
                        break;
                    case MIDI_META_EVENT_COPYRIGHT:
                        m_metaTextInfo.copyright = std::string(event->event.meta.text.val);
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] copyright: " << m_metaTextInfo.copyright;
                        break;
                    case MIDI_META_EVENT_TRACK_NAME:
                        m_metaTextInfo.track = std::string(event->event.meta.text.val);
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] track: " << m_metaTextInfo.track;
                        break;
                    case MIDI_META_EVENT_INSTRUMENT_NAME:
                        m_metaTextInfo.instrument = std::string(event->event.meta.text.val);
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] instrument: " << m_metaTextInfo.instrument;
                        break;
                    case MIDI_META_EVENT_LYRIC_TEXT:
                        m_metaTextInfo.lyric = std::string(event->event.meta.text.val);
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] lyric: " << m_metaTextInfo.lyric;
                        break;
                    case MIDI_META_EVENT_TEXT_MARKER:
                        m_metaTextInfo.marker = std::string(event->event.meta.text.val);
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] marker: " << m_metaTextInfo.marker;
                        break;
                    case MIDI_META_EVENT_CUE_POINT:
                        m_metaTextInfo.cue = std::string(event->event.meta.text.val);
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] cue: " << m_metaTextInfo.cue;
                        break;
                    }
                }
                else
                {
                    BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] found system event: " << Utils::toHex(event->message_meta, 1);
                }
            }
        }
    }
    ChannelAnalyzer &getChannel(const uint8_t channel)
    {
        return m_channels.at(channel);
    }
    const TrackMetaTextInfo &getTrackInfo()
    {
        return m_metaTextInfo;
    }

private:
    Config m_config;
    mtrk_t *m_track = nullptr;
    const float m_pps = 0.0f;
    uint64_t m_trackDuration = 0;
    std::array<ChannelAnalyzer, MIDI_CHANNELS_MAX_COUNT> m_channels;
    TrackMetaTextInfo m_metaTextInfo;
};
