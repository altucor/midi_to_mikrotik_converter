
/*
 +   1) detect note on/off
 +   2) detect delay in ms beetwen notes
 +   3) detect tempo
 +   4) detect ppqn
 +   5) add octave/note/fine shift
 +   6) add multitrack decoding
 +   7) add BPM change
 +   8) add flag -c for writing note comments in file
 +   9) detect full VLV delays
 +  10) detect first/pre delay
 +  11) add logging levels
 -  12) add boost filesystem processing for files
*/

#include "file.h"

#include "Config.hpp"
#include "MikrotikTrack.hpp"
#include "TrackAnalyzer.hpp"

#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup.hpp"
#include <boost/program_options.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

namespace po = boost::program_options;

static void init_log()
{
    static const std::string COMMON_FMT("[%TimeStamp%][%Severity%]: %Message%");

    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

    // Output message to console
    boost::log::add_console_log(std::cout, boost::log::keywords::format = COMMON_FMT, boost::log::keywords::auto_flush = true);

    // Output message to file, rotates when file
    // reached 1mb or at midnight every day. Each log file
    // is capped at 1mb and total is 20mb
    boost::log::add_file_log(boost::log::keywords::file_name = "mtmc_%3N.log", boost::log::keywords::rotation_size = 100 * 1024 * 1024,
                             boost::log::keywords::max_size = 100 * 1024 * 1024,
                             boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
                             boost::log::keywords::format = COMMON_FMT, boost::log::keywords::auto_flush = true);

    boost::log::add_common_attributes();

    // Only output message with INFO or higher severity in Release
#ifndef _DEBUG
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#endif
}

static int32_t parseArguments(int argc, char *argv[], Config &config)
{
    po::options_description desc("Application arguments");
    desc.add_options()                                                                                                                  //
        ("help,h", "Print this help message")                                                                                           //
        ("file,f", po::value<std::string>(&config.inFileName), "Select input standart midi file (SMF)")                                 //
        ("octave-shift,o", po::value<int8_t>(&config.pitchShift.octave), "Sets the octave shift relative to the original (-10 to +10)") //
        ("note-shift,n", po::value<int8_t>(&config.pitchShift.note), "Sets the note shift relative to the original")                    //
        ("fine-tuning,t", po::value<float>(&config.pitchShift.fine),
         "Sets frequency offset for all notes in case when you think your beeper produces beeps at wrong frequencies") //
        ("bpm,b", po::value<int>(&config.bpm), "Sets the new bpm to output file")                                      //
        ("log-level,l", po::value<boost::log::trivial::severity_level>(&config.logLevel)->default_value(boost::log::trivial::severity_level::info),
         "Sets the log level: trace, debug, info, warning, error, fatal") //
        ("comments,c", "Adds comments in the form of notes");

    if (argc == 1)
    {
        std::cout << desc << std::endl;
        return 0;
    }

    po::variables_map vm;
    try
    {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }
        if (vm.count("comments"))
        {
            config.comments = true;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        std::cout << desc << std::endl;
    }

    boost::log::core::get()->set_filter(boost::log::trivial::severity >= config.logLevel);

    if (config.inFileName == "")
    {
        BOOST_LOG_TRIVIAL(error) << "[mtmc] no input (SMF) file specified";
        std::cout << desc << std::endl;
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    init_log();

    Config config = {0};
    int32_t ret = parseArguments(argc, argv, config);
    if (ret < 0)
    {
        return ret;
    }

    std::ifstream midiStream{config.inFileName, midiStream.binary | midiStream.in};
    if (!midiStream.is_open())
    {
        BOOST_LOG_TRIVIAL(error) << "[mtmc] cannot open file: " << config.inFileName;
        return -1;
    }
    std::vector<unsigned char> midiFileBuffer(std::istreambuf_iterator<char>(midiStream), {});

    midi_file_t *midiFile = midi_file_new();

    int read = midi_file_unmarshal(midiFile, midiFileBuffer.data(), midiFileBuffer.size());
    if (read > 0)
    {
        BOOST_LOG_TRIVIAL(info) << "[mtmc] midi file parsed total bytes: " << std::to_string(read) << std::endl;
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "[mtmc] error parsing midi file stream: " << std::to_string(read) << std::endl;
        midi_file_free(midiFile);
        return -1;
    }

    config.ppqn = midi_file_get_mthd(midiFile).ppqn;
    BOOST_LOG_TRIVIAL(info) << "[mtmc] mthd ppqn: " << std::to_string(config.ppqn);
    const uint16_t trackCount = midi_file_get_tracks_count(midiFile);
    BOOST_LOG_TRIVIAL(info) << "[mtmc] track count: " << std::to_string(trackCount);

    // If new BPM not set by user input find in one of tracks
    if (config.bpm == 0)
    {
        BOOST_LOG_TRIVIAL(info) << "[mtmc] searching bpm event...";
        for (uint32_t i = 0; i < trackCount; i++)
        {
            mtrk_t *track = midi_file_get_track(midiFile, i);

            // midi_cmd_t cmd = {.status = MIDI_STATUS_SYSTEM, .subCmd = MIDI_STATUS_SYSTEM_RESET_OR_META};
            midi_cmd_t cmd = {MIDI_STATUS_SYSTEM_RESET_OR_META, MIDI_STATUS_SYSTEM};
            uint8_t message_meta = MIDI_META_EVENT_TEMPO;
            uint32_t bpmIndex = mtrk_find_event_index(track, 0, cmd, message_meta);
            if (bpmIndex != -1)
            {
                midi_event_smf_t *bpmEvent = mtrk_get_event(track, bpmIndex);
                config.bpm = bpmEvent->event.meta.tempo.val;
            }
        }
    }

    BOOST_LOG_TRIVIAL(info) << "[mtmc] bpm set to: " << std::to_string(config.bpm);

    std::vector<MikrotikTrack> mikrotikTracks;

    for (uint16_t i = 0; i < trackCount; i++)
    {
        TrackAnalyzer trackAnalyzer(config, i);
        mtrk_t *track = midi_file_get_track(midiFile, i);
        BOOST_LOG_TRIVIAL(info) << "[mtmc] analyzing track: " << std::to_string(i);
        auto tracks = trackAnalyzer.analyze(track);
        mikrotikTracks.insert(mikrotikTracks.end(), tracks.begin(), tracks.end());
        BOOST_LOG_TRIVIAL(info) << "[mtmc] analyzing track: " << std::to_string(i) << " done";
    }

    midi_file_free(midiFile);

    // for (std::size_t i = 0; i < mikrotikTracks.size(); i++)
    // {
    //     mikrotikTracks[i].exportScript();
    // }

    BOOST_LOG_TRIVIAL(info) << "[mtmc] FILE name in: " << config.inFileName;
    std::for_each(mikrotikTracks.begin(), mikrotikTracks.end(), [&](MikrotikTrack &track) { track.exportScript(config); });

    return 0;
}
