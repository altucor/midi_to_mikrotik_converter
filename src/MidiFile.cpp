#include "MidiFile.h"

/*
MThd - 77 84 104 100
MTrk - 77 84 114 107
*/

MidiFile::MidiFile( const std::string &_fp, const int &_octave_shift, const int &_note_shift, const int &_new_bpm, const bool &_comments_flag ) :
    file_path(_fp),
    octave_shift(_octave_shift),
    note_shift(_note_shift),
    new_bpm(_new_bpm),
    comments_flag(_comments_flag)
{
    //ctor

    if( _readfile( file_path ) == 1){
        std::cout << "Success opened file" << std::endl;
    } else {
        std::cout << " @ERROR: file can not be opened" << std::endl;
        exit(-1);
    }

    if( _find_chunks_and_decode() == 1){
        std::cout << "Standart Midi File structure detected" << std::endl;
    } else {
        std::cout << " @ERROR: This file does not have a (Standart Midi File) structure " << std::endl;
        exit(-1);
    }

    _humanize_time();
}

MidiFile::~MidiFile()
{
    //dtor
}

int MidiFile::_readfile( std::string path ){
    std::streampos sz, begin, end;
    char * memblock;

    std::ifstream midi_file_stream( path, std::ios::in | std::ios::binary | std::ios::ate );

    if (midi_file_stream.is_open())
    {
        sz = midi_file_stream.tellg();
        memblock = new char [sz];
        midi_file_stream.seekg (0, std::ios::beg);
        begin =  midi_file_stream.tellg();
        midi_file_stream.read (memblock, sz);
        midi_file_stream.seekg (0, std::ios::end);
        end =  midi_file_stream.tellg();
        midi_file_stream.close();

        file_size = (end-begin);

        for(int i=0; i < file_size; i++){
            file_data.push_back(memblock[i]);
        }

        delete[] memblock;
        is_file_success_readed = true;
        return 1;
    } else {
        is_file_success_readed = false;
        return 0;
    }
}

int MidiFile::_find_chunks_and_decode(){
    fptr = 0;
    if( file_data[fptr++] == 77 &&      // M
        file_data[fptr++] == 84 &&      // T
        file_data[fptr++] == 104 &&     // h
        file_data[fptr++] == 100 ){     // d
        std::cout << "MThd ID OK" << std::endl;
    } else {
        return 0;
    }

    mthd_chunk_length  = (unsigned char)file_data[fptr++] << 24;
    mthd_chunk_length |= (unsigned char)file_data[fptr++] << 16;
    mthd_chunk_length |= (unsigned char)file_data[fptr++] << 8;
    mthd_chunk_length |= (unsigned char)file_data[fptr++];

    std::cout << "MThd Chunk Length: " << std::dec << mthd_chunk_length << std::endl;

    format_type  = (unsigned char)file_data[fptr++] << 8;
    format_type |= (unsigned char)file_data[fptr++];
    std::cout << "Format type: " << std::dec << format_type << std::endl;

    mtrk_chunks_cnt  = (unsigned char)file_data[fptr++] << 8;
    mtrk_chunks_cnt |= (unsigned char)file_data[fptr++];
    std::cout << "MTrk chunks cnt: " << std::dec << mtrk_chunks_cnt << std::endl;

    ppqn  = (unsigned char)file_data[fptr++] << 8;
    ppqn |= (unsigned char)file_data[fptr++];
    std::cout << "PPQN: " << std::dec << ppqn << std::endl;


    MTrkChunk *MTrk = new MTrkChunk[mtrk_chunks_cnt];

    for( int i=0; i < mtrk_chunks_cnt; i++){

        if( i == 0){
            if( MTrk[i].setInitData( fptr, file_data, octave_shift, note_shift ) == 1){
                std::cout << "MTrk chunk #" << i << " found\n";
            } else {
                std::cout << " @ERROR: MTrk chunk NOT #" << i << " found\n";
            }
        } else {
            if( MTrk[i].setInitData( MTrk[i-1].getEndPos(), file_data, octave_shift, note_shift ) == 1 ){
                std::cout << "MTrk chunk #" << i << " found\n";
            } else {
                std::cout << " @ERROR: MTrk chunk NOT #" << i << " found\n";
            }
        }


        if( MTrk[i].getTempo() > 0 ){
            tempto_track = MTrk[i].getTempo();
        }

        if( new_bpm > 0 ){
            tempto_track = new_bpm;
        }

        std::cout << "BPM: " << tempto_track << std::endl;
        std::cout << "Notes ON Count in MTrk chunk: " << MTrk[i].getEventsNoteONCount() << std::endl;
        std::cout << "Notes OFF Count in MTrk chunk: " << MTrk[i].getEventsNoteOFFCount() << std::endl;
        std::cout << "Track Name: " << MTrk[i].getTrackName() << std::endl;
        std::cout << "Track Text: " << MTrk[i].getTrackText() << std::endl;
        std::cout << "Note Channel: " << MTrk[i].getNoteChannel() << std::endl;

        int note_cnt_difference =  MTrk[i].getEventsNoteONCount() - MTrk[i].getEventsNoteOFFCount();

        if( note_cnt_difference == 0 && MTrk[i].getEventsNoteONCount() > 0 && MTrk[i].getEventsNoteOFFCount() > 0 ){
            pulses_per_sec = (double)60000 / ( tempto_track * ppqn );
            std::cout << "Pulses per second: " << pulses_per_sec << std::endl;

            freq_notes_arr = MTrk[i].getFreqNotes();
            length_notes_on_arr = MTrk[i].getLengthNotesON();
            length_notes_off_arr = MTrk[i].getLengthNotesOFF();
            symbolic_notes_arr = MTrk[i].getSymbolicNotes();
            //first_dly = MTrk[i].getFirstDelay();
            _create_mikrotik_script_file( i, MTrk[i].getTrackText(), MTrk[i].getTrackName(), MTrk[i].getEventsNoteONCount(), MTrk[i].getNoteChannel() );
        }
        std::cout << std::endl;
    }
    return 1;
}

int MidiFile::_create_mikrotik_script_file( const int &chunk_number, const std::string &track_text, const std::string &track_name_comment, const int &notes_cnt, const int &note_ch ){
    /*
    :beep frequency=440 length=1000ms;
    :delay 1000ms;
    */
    //cout << "WRITE FUNC RUN: " << chunk_number << endl;
    std::ostringstream ost_chunk_number;
    ost_chunk_number << chunk_number;

    std::ostringstream ost_bpm;
    ost_bpm << tempto_track;

    std::ostringstream ost_notes_cnt;
    ost_notes_cnt << notes_cnt;

    std::ostringstream ost_note_ch;
    ost_note_ch << note_ch;

    //ostringstream ost_first_dly;


    int status = 0;
    std::string spaces = "";
    std::string out_data = "";
    std::string out_path = file_path + ost_chunk_number.str() +".txt";;
    std::ofstream output_file;

    out_data += "#----------------File Description-----------------#\n";
    out_data += "# This file created by Midi To Mikrotik Converter\n";
    out_data += "# Original midi file name/path: " + file_path + "\n";
    out_data += "# Track BPM: " + ost_bpm.str() + "\n";
    out_data += "# MIDI Channel: " + ost_note_ch.str() + "\n";
    out_data += "# Number of notes: " + ost_notes_cnt.str() + "\n";
    out_data += "# Track text: " + track_text + "\n";
    out_data += "# Track comment: " + track_name_comment + "\n";
    out_data += "#-------------------------------------------------#\n\n";

    //out_data += ":delay " + ost_st_dly.str() + "ms;\n\n";

    /*
    double first_dly_double = 0;
    for( int j=0; j<first_dly.size(); j++){
        cout << " DBG: " << first_dly[j] << endl;
        first_dly_double += ( first_dly[j] * pulses_per_sec );
    }
    ost_first_dly << first_dly_double;
    out_data += ":delay " + ost_first_dly.str() + "ms;\n\n";
    */

    for( uint64_t i=0; i<freq_notes_arr.size(); i++){
        std::string comment_str = "";

        std::ostringstream ost_freq;
        std::ostringstream ost_note_duration_ms;
        std::ostringstream ost_pause_duration_ms;

        ost_freq << freq_notes_arr[i];

        double note_on_duration_ms = 0;
        for( uint64_t j=0; j<length_notes_on_arr[i].size(); j++){
            note_on_duration_ms += ( length_notes_on_arr[i][j] * pulses_per_sec );
        }

        double pause_duration_ms = note_on_duration_ms;
        for( uint64_t j=0; j<length_notes_off_arr[i].size(); j++){
            pause_duration_ms += (length_notes_off_arr[i][j] * pulses_per_sec);
        }

        full_track_length += note_on_duration_ms + pause_duration_ms;

        ost_note_duration_ms << note_on_duration_ms;
        ost_pause_duration_ms << pause_duration_ms;

        if( comments_flag == true ){
            comment_str = " # " + symbolic_notes_arr[i];
        }

        out_data += ":beep frequency=" + ost_freq.str() + " length=" + ost_note_duration_ms.str() + "ms;" + comment_str + "\n:delay " + ost_pause_duration_ms.str() + "ms;\n\n";
    }


    output_file.open(out_path);
    if(output_file.is_open()){
        status = 1;
    } else {
        status = 0;
    }
    output_file << out_data;
    output_file.close();

    return status;
}

void MidiFile::_humanize_time(){
    if( full_track_length > 60 ){
        ftl_m = full_track_length / 60;

        if( full_track_length > 60 ){
            ftl_h = full_track_length / 3600;
        }
    }

    //cout << "Hours: " << ftl_h << "Minutes: " << ftl_m << "Seconds: " << ftl_s << endl;
}

std::string MidiFile::getPath(){
    return file_path;
}

int MidiFile::getFileSize(){
    return file_size;
}

std::vector<unsigned char> MidiFile::getFileData(){
    return file_data;
}

double MidiFile::getFullTrackLength(){
    return full_track_length;
}
