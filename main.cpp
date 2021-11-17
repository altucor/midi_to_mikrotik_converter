
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

#include "./include/MidiFile.h"
#include "./include/MTrkChunk.h"

#include <boost/program_options.hpp>
#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup.hpp"

namespace po = boost::program_options;

static void init_log()
{
	static const std::string COMMON_FMT("[%TimeStamp%][%Severity%]: %Message%");

	boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");

	// Output message to console
	boost::log::add_console_log(
		std::cout,
		boost::log::keywords::format = COMMON_FMT,
		boost::log::keywords::auto_flush = true
	);

	// Output message to file, rotates when file reached 1mb or at midnight every day. Each log file
	// is capped at 1mb and total is 20mb
	boost::log::add_file_log (
		boost::log::keywords::file_name = "mtmc_%3N.log",
		boost::log::keywords::rotation_size = 1 * 1024 * 1024,
		boost::log::keywords::max_size = 20 * 1024 * 1024,
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
		boost::log::keywords::format = COMMON_FMT,
		boost::log::keywords::auto_flush = true
	);

	boost::log::add_common_attributes();

	// Only output message with INFO or higher severity in Release
#ifndef _DEBUG
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::info
	);
#endif

}

int main(int argc, char *argv[])
{
	init_log();

	int result = 0;
	int octaveShift = 0;
	int noteShift = 0;
	std::string fileName = "";
	int newBpm = -1;
	int debugLevel = 0;
	bool enableCommentsFlag = false;
	bool predelayFlag = false;
	double fineTuning = 0.0;

	po::options_description desc("Application arguments");
	desc.add_options()
		("help,h", "Print this help message")
		("debug,d", po::value<int>(&debugLevel), "Enable verbose debug prints, allowed levels 0-5")
		("file,f", po::value<std::string>(&fileName), "Select input standart midi file (SMF)")
		("octave-shift,o", po::value<int>(&octaveShift), "Sets the octave shift relative to the original (-10 to +10)")
		("note-shift,n", po::value<int>(&noteShift), "Sets the note shift relative to the original")
		("fine-tuning,t", po::value<double>(&fineTuning), "Sets frequency offset for all notes in case when you think your beeper producess beeps at wrong frequencies")
		("bpm,b", po::value<int>(&newBpm), "Sets the new bpm to output file")
		("comments,c", po::value<bool>(&enableCommentsFlag), "Adds comments in the form of notes")
		("predelay,p", po::value<bool>(&predelayFlag), "Enable pre-delay in track")
	;

	if(argc == 1)
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
	}
	catch(std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		std::cout << desc << std::endl;
	}

	/*
	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
	*/

	if( fileName != "" )
	{
		MidiFile midiObj;
		midiObj.setFilePath(fileName);
		midiObj.setOctaveShift(octaveShift);
		midiObj.setNoteShift(noteShift);
		midiObj.setNewBpm(newBpm);
		midiObj.setDebugLevel(debugLevel);
		midiObj.setEnableCommentsTo(enableCommentsFlag);
		midiObj.setPredelayFlag(predelayFlag);
		midiObj.parseFile();
	} 
	else 
	{
		BOOST_LOG_TRIVIAL(error) << "No file specified";
	}

	return 0;
}
