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

class MikrotikNote
{
public:
    MikrotikNote(const uint8_t pitch, const uint32_t duration) : m_pitch(pitch), m_duration(duration)
    {
    }
    MikrotikNote(midi_note_t &noteOn, midi_note_t &noteOff, const uint32_t duration)
    {
        // same kind of events skip
        if (noteOn.on == noteOff.on)
        {
            return;
        }
        // different channel events
        if (noteOn.channel != noteOff.channel)
        {
            return;
        }
        // zero pitch ignore
        if (noteOn.pitch == 0 || noteOff.pitch == 0)
        {
            return;
        }
        // invalid pitch
        if (noteOn.pitch != noteOff.pitch)
        {
            return;
        }
        m_pitch = noteOn.pitch;
        m_duration = duration;
    }
    void log() const
    {
        BOOST_LOG_TRIVIAL(info) << "Note " << " pitch: " << (uint32_t)m_pitch << " duration ticks: " << (uint32_t)m_duration;
    }
    std::string toString(const PitchShift pitchShift, const float pps, const bool comments)
    {
        /*
         * :beep frequency=440 length=1000ms; # C4 + 35Hz
         * :delay 1000ms;
         */

        uint8_t shiftedPitch = pitchShift.process(m_pitch);
        float freq = pitch_to_freq(shiftedPitch) + pitchShift.fine;

        std::stringstream ss;
        ss << ":beep frequency=" << std::to_string(freq);
        ss << " length=" << duration_to_ms(m_duration, pps) << "ms;";

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
        ss << "\n";

        return ss.str();
    }
    std::string toStringWithDelay(const PitchShift pitchShift, const float pps, const bool comments)
    {
        return toString(pitchShift, pps, comments) + getDelayLine(duration_to_ms(m_duration, pps)) << "\n";
    }
    uint32_t duration() const
    {
        return m_duration;
    }

private:
    uint8_t m_pitch = 0;
    uint32_t m_duration = 0;
};
