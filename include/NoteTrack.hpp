#pragma once

#include "event.h"

#include "Note.hpp"

#include <vector>

class NoteTrack
{
public:
    NoteTrack(const uint8_t channel) : m_channel(channel)
    {
    }
    bool addEvent(midi_event_smf_t event)
    {
        m_durationAccumulator += event.predelay.val;

        if (event.message.status != MIDI_STATUS_NOTE_OFF && event.message.status != MIDI_STATUS_NOTE_ON)
        {
            return false;
        }

        if (m_notes.size() == 0)
        {
            if (event.message.status == MIDI_STATUS_NOTE_OFF)
            {
                return false;
            }
        }

        if (event.event.note.on)
        {
            if (m_noteOn.pitch != 0)
            {
                return false;
            }
            m_durationAccumulator = 0;
            m_noteOn = event.event.note;
            return true;
        }
        else
        {
            if (m_noteOn.pitch != event.event.note.pitch)
            {
                return false;
            }

            m_noteOff = event.event.note;
            m_notes.push_back(Note(m_noteOn, m_noteOff, m_durationAccumulator));
            m_durationAccumulator = 0;
            m_noteOn = {0};
            m_noteOff = {0};
            return true;
        }

        return false;
    }

private:
    uint8_t m_channel = 0;
    uint32_t m_durationAccumulator = 0;
    midi_note_t m_noteOn = {0};
    midi_note_t m_noteOff = {0};
    std::vector<Note> m_notes;
};
