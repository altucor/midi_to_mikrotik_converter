#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include "MTrkChunk.h"

using namespace std;

class MidiFile
{
    public:
        // Persists

        // Functions
        MidiFile(void);
        MidiFile( string _fp, int _octave_shift, int _note_shift, int _new_bpm, bool _comments_flag );
        virtual ~MidiFile();
        string getPath();
        int getFileSize();
        vector<unsigned char> getFileData();

        double getFullTrackLength();

    protected:

    private:
        // Persists
        std::string file_path;
        int octave_shift = 0;
        int note_shift = 0;
        int new_bpm = -1;
        bool comments_flag = false;
        int file_size = 0;
        int fptr = 0; //file pointer
        vector<unsigned char> file_data;
        //vector<struct> mtrk_chunks_arr;
        //int mtrk_chunk_length = 0;
        bool is_file_success_readed = false;
        int mthd_chunk_length = 0;
        int format_type = 0;
        int mtrk_chunks_cnt = 0;
        int ppqn = 0;
        int tempto_track = 0;
        double pulses_per_sec = 0.0;
        double full_track_length = 0.0; // in seconds (like timestamp)
        int ftl_h = 0; //ftl - full_track_length in hours
        int ftl_m = 0; //ftl - full_track_length in minutes
        int ftl_s = 0; //ftl - full_track_length in secondss
        vector<double> freq_notes_arr;
        vector<vector<int>> length_notes_on_arr;
        vector<vector<int>> length_notes_off_arr;
        vector<string> symbolic_notes_arr;
        vector<int> first_dly;

        // Functions
        int _readfile( string path);
        int _find_chunks_and_decode();
        int _create_mikrotik_script_file( int chunk_number, string track_text, string track_name_comment, int notes_cnt, int note_ch );
        void _humanize_time();
};

#endif // MIDIFILE_H
