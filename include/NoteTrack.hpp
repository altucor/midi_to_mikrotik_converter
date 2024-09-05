#pragma once

#include "event.h"

#include <vector>

class NoteTrack
{
public:
    NoteTrack();
    bool addEvent(midi_event_smf_t *event)
    {
        if (event->message.status != MIDI_STATUS_NOTE_OFF && event->message.status != MIDI_STATUS_NOTE_ON)
        {
            // but anyway append delay
            m_durationAccumulator += event->predelay.val;
            return false;
        }
        if (m_notes.size() == 0 && event->message.status == MIDI_STATUS_NOTE_OFF)
        {
            return false;
        }
        if (m_notes.size() > 0)
        {
            if (m_notes.end()->on == event->event.note.on || m_notes.end()->pitch != event->event.note.pitch)
            {
                return false;
            }
        }

        // m_durationAccumulator
        m_notes.push_back(event->event.note);
        m_durationAccumulator = 0;
        return true;
    }

private:
    uint32_t m_durationAccumulator = 0;
    std::vector<midi_note_t> m_notes;
};
