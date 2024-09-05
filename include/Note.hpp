#pragma once

#include "events/note.h"
#include "util.h"

#include <boost/log/trivial.hpp>
#include <sstream>

class PitchShift
{
public:
    uint8_t octave = 0;
    uint8_t note = 0;
    float fine = 0.0f;
    uint8_t process(const uint8_t pitch) const
    {
        return pitch + (octave * MIDI_NOTES_IN_OCTAVE) + note;
    }
};

class Note
{
public:
    Note(midi_note_t &note_on, midi_note_t &note_off, const float pps) : m_pps(pps)
    {
        // same kind of events skip
        if (note_on.on == note_off.on)
        {
            return;
        }
        // different channel events
        if (note_on.channel != note_off.channel)
        {
            return;
        }
        // zero pitch ignore
        if (note_on.pitch == 0 || note_off.pitch == 0)
        {
            return;
        }
        // invalid pitch
        if (note_on.pitch != note_off.pitch)
        {
            return;
        }
        m_channel = note_on.channel;
        m_pitch = note_on.pitch;
        m_duration = note_off.velocity;
    }
    void log() const
    {
        BOOST_LOG_TRIVIAL(info) << "Note "
                                << " channel: " << (uint32_t)m_channel << " pitch: " << (uint32_t)m_pitch << " duration ticks: " << (uint32_t)m_duration;
    }
    std::string toMikrotikBeep(const PitchShift pitchShift, const bool comments)
    {
        /*
         * :beep frequency=440 length=1000ms;
         * :delay 1000ms;
         */

        uint8_t shiftedPitch = pitchShift.process(m_pitch);
        float freq = pitch_to_freq(shiftedPitch) + pitchShift.fine;

        std::stringstream ss;
        ss << ":beep frequency=" << std::to_string(freq);
        ss << " length=" << duration_to_ms(m_duration, m_pps) << "ms;";

        if (comments)
        {
            ss << " # " << std::string(pitch_to_name(shiftedPitch));
            if (pitchShift.fine != 0.0f)
            {
                ss << std::string(" ");
                ss << ((pitchShift.fine < 0.0f) ? "-" : "+");
                ss << pitchShift.fine;
                ss << std::string(" Hz");
            }
            // ss << " @ " << getTimeAsText(m_processedTime);
        }
        return ss.str();
    }
    uint32_t duration() const
    {
        return m_duration;
    }

private:
    float m_pps;
    uint8_t m_channel = 0;
    uint8_t m_pitch = 0;
    uint32_t m_duration;
};
