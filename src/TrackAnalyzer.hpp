#pragma once

#include "mtrk.h"

#include "Sequence.hpp"
// #include "ChannelAnalyzer.hpp"
#include "Config.hpp"

#include "boost/log/trivial.hpp"

#include <array>
#include <vector>

class TrackAnalyzer
{
public:
    TrackAnalyzer(Config &config, const std::size_t trackIndex) : m_config(config), m_trackIndex(trackIndex)
    {
        for (uint8_t i = 0; i < MIDI_CHANNELS_MAX_COUNT; i++)
        {
            m_sequences[i] = Sequence(m_trackIndex, i);
        }
    }
    std::vector<MikrotikTrack> analyze(mtrk_t *track)
    {
        if (track == nullptr)
        {
            return std::vector<MikrotikTrack>();
        }
        uint32_t eventsCount = mtrk_get_events_count(track);
        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] available midi events: " << std::to_string(eventsCount);
        for (uint32_t i = 0; i < eventsCount; i++)
        {
            midi_event_smf_t *event = mtrk_get_event(track, i);
            m_trackDuration += event->predelay.val;
            if (event->message.status == MIDI_STATUS_NOTE_OFF || event->message.status == MIDI_STATUS_NOTE_ON)
            {
                midi_note_t note = event->event.note;
                m_sequences.at(note.channel).appenMidiNoteEvent(note.on, note.pitch, m_trackDuration);
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
                    switch (event->message_meta)
                    {
                    case MIDI_META_EVENT_TRACK_END:
                        BOOST_LOG_TRIVIAL(info) << "[TrackAnalyzer] system event END OF TRACK";
                        break;
                    }
                }
            }
        }
        // for (uint8_t i = 0; i < MIDI_CHANNELS_MAX_COUNT; i++)
        // {
        //     // m_sequences.at(i).setTrackInfo(m_metaTextInfo);
        //     m_sequences.at(i).analyze();
        // }

        std::vector<MikrotikTrack> outTracks;
        std::for_each(m_sequences.begin(), m_sequences.end(), [&](Sequence &seq) {
            auto tracks = seq.analyze();
            outTracks.insert(outTracks.end(), tracks.begin(), tracks.end());
        });

        return outTracks;
    }

private:
    Config m_config;
    std::size_t m_trackIndex = 0;
    const float m_pps = 0.0f;
    uint64_t m_trackDuration = 0;
    std::array<Sequence, MIDI_CHANNELS_MAX_COUNT> m_sequences;
    TrackMetaTextInfo m_metaTextInfo;
};
