
/*
 +   1) detect note on
 +   2) detect note off
 +   3) detect delay in ms beetwen notes
 +   4) detect tempo
 +   5) detect ppqn
 +   6) add octave shift
 +   7) add note shift
 +   8) add multitrack decoding
 +   9) add BPM change
 +  10) auto detect note channel
 +  11) add flag -c for writing note comments in file
 +  12) detect more note length bytes before 0x90 or 0x80
+/- 13) detect first/intro delay bytes (works but some times not so good)
*/

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstdio>
#include <vector>

// #include "MidiFile.hpp"
#include "file.h"
#include "Mikrotik.hpp"

#include <boost/program_options.hpp>
#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup.hpp"

namespace po = boost::program_options;

static void init_log()
{
	static const std::string COMMON_FMT("[%TimeStamp%][%Severity%]: %Message%");

	boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

	// Output message to console
	boost::log::add_console_log(
		std::cout,
		boost::log::keywords::format = COMMON_FMT,
		boost::log::keywords::auto_flush = true);

	// Output message to file, rotates when file
	// reached 1mb or at midnight every day. Each log file
	// is capped at 1mb and total is 20mb
	boost::log::add_file_log(
		boost::log::keywords::file_name = "mtmc_%3N.log",
		boost::log::keywords::rotation_size = 100 * 1024 * 1024,
		boost::log::keywords::max_size = 100 * 1024 * 1024,
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
		boost::log::keywords::format = COMMON_FMT,
		boost::log::keywords::auto_flush = true);

	boost::log::add_common_attributes();

	// Only output message with INFO or higher severity in Release
#ifndef _DEBUG
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::info);
#endif
}

int main(int argc, char *argv[])
{
	init_log();

	int32_t result = 0;
	int32_t octaveShift = 0;
	int32_t noteShift = 0;
	std::string inFileName = "";
	std::string outFileName = "";
	int32_t newBpm = 0;
	bool enableCommentsFlag = false;
	double fineTuning = 0.0;

	po::options_description desc("Application arguments");
	desc.add_options()("help,h", "Print this help message")("file,f", po::value<std::string>(&inFileName), "Select input standart midi file (SMF)")("output-file,g", po::value<std::string>(&outFileName), "Specify output file name. If not set than output will be saved in file with same name as input but with additional suffix")("octave-shift,o", po::value<int>(&octaveShift), "Sets the octave shift relative to the original (-10 to +10)")("note-shift,n", po::value<int>(&noteShift), "Sets the note shift relative to the original")("fine-tuning,t", po::value<double>(&fineTuning), "Sets frequency offset for all notes in case when you think your beeper produces beeps at wrong frequencies")("bpm,b", po::value<int>(&newBpm), "Sets the new bpm to output file")("comments,c", "Adds comments in the form of notes");

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
			enableCommentsFlag = true;
	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << std::endl;
		std::cout << desc << std::endl;
	}

	if (inFileName == "")
	{
		BOOST_LOG_TRIVIAL(error) << "No input (SMF) file specified";
		std::cout << desc << std::endl;
		return -1;
	}

	if (outFileName == "")
	{
		outFileName = inFileName;
	}

	std::ifstream midiStream{inFileName, midiStream.binary | midiStream.in};
	if (!midiStream.is_open())
	{
		BOOST_LOG_TRIVIAL(error) << "Cannot open file: " << inFileName;
		return -1;
	}
	std::vector<unsigned char> midiFileBuffer(std::istreambuf_iterator<char>(midiStream), {});

	midi_file_t *midiFile = midi_file_new();

	int read = midi_file_unmarshal(midiFile, midiFileBuffer.data(), midiFileBuffer.size());
	if (read > 0)
	{
		BOOST_LOG_TRIVIAL(info) << "Midi file parsed total bytes: " << std::to_string(read) << std::endl;
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Error parsing midi file stream: " << std::to_string(read) << std::endl;
		midi_file_free(midiFile);
		return -1;
	}

	uint16_t ppqn = midi_file_get_mthd(midiFile).ppqn;

	// If new not set by user input find in one of tracks
	if (newBpm == 0)
	{
		auto trackCount = midi_file_get_tracks_count(midiFile);
		for (uint32_t i = 0; i < trackCount; i++)
		{
			mtrk_t *track = midi_file_get_track(midiFile, i);

			midi_cmd_t cmd = {.status = MIDI_STATUS_SYSTEM, .subCmd = MIDI_STATUS_SYSTEM_RESET_OR_META};
			uint8_t message_meta = MIDI_META_EVENT_TEMPO;
			uint32_t bpmIndex = mtrk_find_event_index(track, 0, cmd, message_meta);
			if (bpmIndex != -1)
			{
				midi_event_smf_t *bpmEvent = mtrk_get_event(track, bpmIndex);
				newBpm = bpmEvent->event.meta.tempo.val;
			}
		}
	}

	auto trackCount = midi_file_get_tracks_count(midiFile);

	for (uint32_t i = 0; i < trackCount; i++)
	{
		mtrk_t *track = midi_file_get_track(midiFile, i);
		Mikrotik mikrotik(
			track,
			ppqn,
			newBpm,
			i,
			octaveShift,
			noteShift,
			fineTuning,
			enableCommentsFlag);
		mikrotik.buildScript(outFileName);
	}

	midi_file_free(midiFile);

	// MidiFile midiObj(inFileName, newBpm);
	// midiObj.process();
	// std::vector<MidiTrack> tracks = midiObj.getTracks();

	// for (uint64_t i = 0; i < tracks.size(); i++)
	// {
	// 	Mikrotik mikrotik(
	// 		tracks[i],
	// 		i,
	// 		octaveShift,
	// 		noteShift,
	// 		fineTuning,
	// 		enableCommentsFlag);
	// 	mikrotik.buildScript(outFileName);
	// }

	return 0;
}
