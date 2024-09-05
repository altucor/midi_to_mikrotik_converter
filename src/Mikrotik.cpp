#include "Mikrotik.hpp"

#include "util.h"

#include "Utils.hpp"
#include "boost/log/trivial.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

std::string Mikrotik::getTrackTimeLength(const uint8_t channel)
{
    std::stringstream out;
    out << Utils::getTimeAsText(duration_to_ms(mtrk_get_duration(m_track), m_pps));
    out << " HH:MM:SS:MS";
    return out.str();
}

std::string Mikrotik::getNotesCount(const uint8_t channel)
{
    uint64_t count = 0;
    std::stringstream out;
    midi_cmd_t cmd = {.status = MIDI_STATUS_NOTE_ON, .subCmd = channel};
    uint8_t message_meta = 0;
    uint32_t eventIndex = mtrk_find_event_index(m_track, 0, cmd, message_meta);
    while (eventIndex != -1)
    {
        count++;
        eventIndex = mtrk_find_event_index(m_track, eventIndex + 1, cmd, message_meta);
    }
    out << count;
    return out.str();
}

void Mikrotik::getScriptHeader(std::stringstream &out, const uint8_t channel)
{
    std::stringstream outputBuffer;
    out << "#----------------File Description-----------------#\n";
    out << "# This file generated by Midi To Mikrotik Converter\n";
    out << "# Visit app repo: https://github.com/altucor/midi_to_mikrotik_converter\n";
    out << "# Original midi file name/path: " << m_config.inFileName << "\n";
    out << "# Track BPM: " << std::to_string(m_config.bpm) << "\n";
    out << "# MIDI Channel: " << std::to_string(channel) << "\n";
    out << "# Number of notes: " << getNotesCount(channel) << "\n";
    out << "# Track length: " << getTrackTimeLength(channel) << "\n";
    // out << "# Track name: " << m_track.getName() << "\n";
    // out << "# Instrument name: " << m_track.getInstrumentName() << "\n";
    // out << "# Track text: " << chunk.mtrkChunkHandler.getTrackText() << "\n";
    // out << "# Track copyright: " << chunk.mtrkChunkHandler.getCopyright() << "\n";
    // out << "# Vocals: " << chunk.mtrkChunkHandler.getInstrumentName() << "\n";
    // out << "# Text marker: " << chunk.mtrkChunkHandler.getTextMarker() << "\n";
    // out << "# Cue Point: " << chunk.mtrkChunkHandler.getCuePoint() << "\n";
    out << "#-------------------------------------------------#\n\n";
}

int Mikrotik::buildScriptForChannel(std::string &fileName, const uint8_t channel)
{
    m_processedTime = 0.0;
    std::string outFileName(fileName);
    outFileName += std::string("_");
    // outFileName += m_track.getName();
    outFileName += std::string("_");
    outFileName += std::to_string(m_index);
    outFileName += std::string("_");
    outFileName += std::to_string(channel);
    outFileName += std::string(".txt");

    std::stringstream outputBuffer;
    getScriptHeader(outputBuffer, channel);

    uint64_t foundNoteEventsCount = 0;
    float pps = pulses_per_second(m_config.ppqn, m_config.bpm);

    uint32_t eventsCount = mtrk_get_events_count(m_track);
    for (uint32_t i = 0; i < eventsCount; i++)
    {
        midi_event_smf_t *event = mtrk_get_event(m_track, i);
        if (event->message.subCmd != channel)
        {
            continue;
        }
        if (event->predelay.val != 0)
        {
            // outputBuffer << getDelayLine(duration_to_ms(event->predelay.val, pps));
        }
        if (event->message.status == MIDI_STATUS_NOTE_ON)
        {
            int32_t noteOffIndex = mtrk_find_corresponding_note_off(m_track, i, *event);
            if (noteOffIndex == -1)
            {
                BOOST_LOG_TRIVIAL(info) << "Cannot find corresponding noteOff event for noteOn at index: " << std::to_string(i);
                continue;
            }

            float noteDuration = 0.0f;
            for (uint32_t j = i; j < noteOffIndex - i; j++)
            {
                midi_event_smf_t *tempEvent = mtrk_get_event(m_track, j);
                noteDuration += duration_to_ms(tempEvent->predelay.val, pps);
            }
            // getBeepLine(event->event.note, noteDuration);
            foundNoteEventsCount++;
        }
    }

    if (foundNoteEventsCount == 0)
    {
        return 0;
    }

    BOOST_LOG_TRIVIAL(info) << "Mikrotik buildScript started for track: " << m_index << " channel: " << (uint32_t)channel;

    std::ofstream outputFile;
    outputFile.open(outFileName);
    if (!outputFile.is_open())
    {
        BOOST_LOG_TRIVIAL(info) << "Mikrotik buildScript failed cannot create output file: " << outFileName;
        return -1;
    }
    outputFile << outputBuffer.str();
    outputFile.close();
    BOOST_LOG_TRIVIAL(info) << "Mikrotik buildScript generated file: " << outFileName;
    return 0;
}

int Mikrotik::buildScript()
{
    // if (m_track.getEvents().size() == 0)
    // {
    // 	BOOST_LOG_TRIVIAL(info) << "Cannot find any events in track, skipping";
    // 	return 0;
    // }

    int ret = 0;
    for (uint8_t channel = 0; channel < MIDI_CHANNELS_MAX_COUNT; channel++)
    {
        ret = buildScriptForChannel(m_config.outFileName, channel);
        if (ret != 0)
        {
            BOOST_LOG_TRIVIAL(info) << "Mikrotik buildScript failed on channel: " << channel;
        }
    }
    return ret;
}
