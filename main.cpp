
/*
+  1) detect note on
+  2) detect note off
+  3) detect delay in ms beetwen notes
+  4) detect tempo
+  5) detect ppqn
+  6) add octave shift
+  7) add note shift
+  8) add multitrack decoding
+  9) add BPM change
+ 10) auto detect note channel
+ 11) add flag -c for writing note comments in file
+ 12) detect more note length bytes before 0x90 or 0x80
- 13) detect first/intro delay bytes
*/

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <cstdio>
#include <vector>
#include <unistd.h>
#include "./include/MidiFile.h"
#include "./include/MTrkChunk.h"

int main(int argc, char *argv[]){
    int result = 0;
    int octaveShift = 0;
    int noteShift = 0;
	std::string fileName;
    //std::string fileName = "H:\\CodeBlocksPRJ\\01.09.2017-01.07.2018\\midi_to_mikrotik_converter\\Debug\\skibidi.mid";
	//std::string fileName = "H:\\CodeBlocksPRJ\\01.09.2017-01.07.2018\\midi_to_mikrotik_converter\\Debug\\Jingle bells.mid";
    int newBpm = -1;
    int debugLevel = 4;
    bool enableCommentsFlag = false;

    while ( (result = getopt(argc,argv,"o:n:f:b:d:c")) != -1){
        switch (result){
            case 'o': octaveShift = atoi(optarg);         break; //octave shift
            case 'n': noteShift = atoi(optarg);           break; //note shift
            case 'f': fileName = optarg;                  break; //midi file
            case 'b': newBpm = atoi(optarg);              break; //set new bpm for all files
            case 'd': debugLevel = atoi(optarg);          break; //enable note comments in output file
            case 'c': enableCommentsFlag = true;          break; //enable note comments in output file
            case '?': printf("Error argument found !\n"); break; //Error handler
        };
	};

    if( fileName != "" ){
        MidiFile midiObj;
        midiObj.setFilePath(fileName);
        midiObj.setOctaveShift(octaveShift);
        midiObj.setNoteShift(noteShift);
        midiObj.setNewBpm(newBpm);
        midiObj.setDebugLevel(debugLevel);
        midiObj.setEnableCommentsTo(enableCommentsFlag);
        midiObj.parseFile();
    } else {
        std::cout << "Error: No file specified" << std::endl;
    }

    return 0;
}
