
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
#include <boost/log/core.hpp>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
	int result = 0;
	int octaveShift = 0;
	int noteShift = 0;
	std::string fileName = "";
	int newBpm = -1;
	int debugLevel = 0;
	bool enableCommentsFlag = false;
	bool predelayFlag = false;

	po::options_description desc("Application arguments");
	desc.add_options()
		("help,h", "Print this help message")
		("debug,d", "Enable verbose debug prints")
		("file,f", po::value<std::string>(&fileName), "Select input standart midi file (SMF)")
		("octave-shift,o", po::value<int>(&octaveShift), "Sets the octave shift relative to the original (-10 to +10)")
		("note-shift,n", po::value<int>(&noteShift), "Sets the note shift relative to the original")
		("bpm,b", po::value<int>(&newBpm), "Sets the new bpm to output file")
		("comments,c", po::value<bool>(&enableCommentsFlag), "Adds comments in the form of notes")
		("predelay,p", po::value<bool>(&predelayFlag), "Enable pre-delay in track")
	;

	

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
	} else {
		std::cout << "Error: No file specified" << std::endl;
	}

	return 0;
}
