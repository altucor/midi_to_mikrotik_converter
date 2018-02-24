#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include "MTrkChunk.h"

//using namespace std;

class MidiFile
{
    public:
        // Persists

        // Functions
        MidiFile( const std::string &_fp, const int &_octave_shift, const int &_note_shift, const int &_new_bpm, const bool &_comments_flag );
        virtual ~MidiFile();
        std::string getPath();
        int getFileSize();
        std::vector<unsigned char> getFileData();

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
        std::vector<unsigned char> file_data;
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
        std::vector<double> freq_notes_arr;
        std::vector<std::vector<int>> length_notes_on_arr;
        std::vector<std::vector<int>> length_notes_off_arr;
        std::vector<std::string> symbolic_notes_arr;
        std::vector<int> first_dly;

        // Functions
        int _readfile( std::string path);
        int _find_chunks_and_decode();
        int _create_mikrotik_script_file( const int &chunk_number, const std::string &track_text, const std::string &track_name_comment, const int &notes_cnt, const int &note_ch );
        void _humanize_time();
};

#endif // MIDIFILE_H
