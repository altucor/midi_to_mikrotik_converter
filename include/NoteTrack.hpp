#pragma once

#include "event.h"

#include "MikrotikNote.hpp"

#include <vector>

enum class NoteRetCode
{
    NOTE_RETCODE_SUCCESS = 0,
    NOTE_RETCODE_NOT_NOTE,
    NOTE_RETCODE_OTHER_CHANNEL,
    NOTE_RETCODE_NO_FIRST_PRESS,
    NOTE_RETCODE_ZERO_PITCH,
    NOTE_RETCODE_OVERLAY,
    NOTE_RETCODE_UNEXPECTED,
};

class NoteTrack
{
public:
    NoteTrack(const uint8_t channel) : m_channel(channel)
    {
    }
    NoteRetCode addEvent(midi_event_smf_t event)
    {
        if (!m_firstAppended)
        {
            m_firstAppended = true;
        }
        else
        {
            // ignore first event pre-delay
            m_durationAccumulator += event.predelay.val;
            m_trackDuration += event.predelay.val;
        }

        if (event.message.status != MIDI_STATUS_NOTE_OFF && event.message.status != MIDI_STATUS_NOTE_ON)
        {
            return NoteRetCode::NOTE_RETCODE_NOT_NOTE;
        }

        if (event.event.note.channel != m_channel)
        {
            return NoteRetCode::NOTE_RETCODE_OTHER_CHANNEL;
        }

        if (m_notes.size() == 0)
        {
            if (event.message.status == MIDI_STATUS_NOTE_OFF)
            {
                return NoteRetCode::NOTE_RETCODE_NO_FIRST_PRESS;
            }
        }

        if (event.event.note.on)
        {
            if (m_noteOn.pitch != 0)
            {
                return NoteRetCode::NOTE_RETCODE_OVERLAY;
            }
            m_durationAccumulator = 0;
            m_noteOn = event.event.note;
            return NoteRetCode::NOTE_RETCODE_SUCCESS;
        }
        else
        {
            if (m_noteOn.pitch != event.event.note.pitch)
            {
                return NoteRetCode::NOTE_RETCODE_OVERLAY;
            }

            m_noteOff = event.event.note;
            m_notes.push_back(Note(m_noteOn, m_noteOff, m_durationAccumulator));
            m_durationAccumulator = 0;
            m_noteOn = {0};
            m_noteOff = {0};
            return NoteRetCode::NOTE_RETCODE_SUCCESS;
        }

        return NoteRetCode::NOTE_RETCODE_UNEXPECTED;
    }

private:
    bool m_firstAppended = false;
    uint8_t m_channel = 0;
    uint64_t m_durationAccumulator = 0;
    uint64_t m_trackDuration = 0;
    midi_note_t m_noteOn = {0};
    midi_note_t m_noteOff = {0};
    std::vector<Note> m_notes;
};
