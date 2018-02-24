
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
#include "MidiFile.h"
#include "MTrkChunk.h"

//using namespace std;

int main(int argc, char *argv[]){
    int res = 0;
    int octave_shift = 0;
    int note_shift = 0;
    std::string file_name = "";
    int new_bpm = -1;
    bool enable_comments_flag = false;

    while ( (res = getopt(argc,argv,"o:n:f:b:c")) != -1){
        switch (res){
            case 'o': octave_shift = atoi(optarg);        break; //octave shift
            case 'n': note_shift = atoi(optarg);          break; //note shift
            case 'f': file_name = optarg;                 break; //midi file
            case 'b': new_bpm = atoi(optarg);             break; //set new bpm for all files
            case 'c': enable_comments_flag = true;        break; //enable note comments in output file
            case '?': printf("Error argument found !\n"); break; //Error handler
        };
	};

    if( file_name != "" ){
        MidiFile midi_obj( file_name, (int)octave_shift, (int)note_shift, (int)new_bpm, enable_comments_flag );
    } else {
        std::cout << "Error: No file specified" << std::endl;
    }

    return 0;
}
