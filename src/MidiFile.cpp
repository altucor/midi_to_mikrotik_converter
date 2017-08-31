#include "MidiFile.h"

/*
MThd - 77 84 104 100
MTrk - 77 84 114 107
*/


MidiFile::MidiFile()
{
    //ctor
}

MidiFile::MidiFile( string _fp, int _octave_shift, int _note_shift, int _new_bpm, bool _comments_flag )
{
    //ctor
    this->file_path = _fp;
    this->octave_shift = _octave_shift;
    this->note_shift = _note_shift;
    this->new_bpm = _new_bpm;
    this->comments_flag = _comments_flag;

    if( this->_readfile( this->file_path ) == 1){
        cout << "Success opened file" << endl;
    } else {
        cout << " @ERROR: file can not be opened" << endl;
        exit(-1);
    }

    if( this->_find_chunks_and_decode() == 1){
        cout << "Standart Midi File structure detected" << endl;
    } else {
        cout << " @ERROR: This file does not have a (Standart Midi File) structure " << endl;
        exit(-1);
    }

    /*
    if( this->_create_mikrotik_script_file() == 1 ){
        cout << "Success created MikroTik script file" << endl;
    } else {
        cout << " @ERROR: MikroTik script file can not be created" << endl;
        exit(-1);
    }
    */

    this->_humanize_time();
}

MidiFile::~MidiFile()
{
    //dtor
}

int MidiFile::_readfile( string path ){
    streampos sz, begin, end;
    char * memblock;

    ifstream midi_file_stream( path, ios::in|ios::binary|ios::ate );

    if (midi_file_stream.is_open())
    {
        sz = midi_file_stream.tellg();
        memblock = new char [sz];
        midi_file_stream.seekg (0, ios::beg);
        begin =  midi_file_stream.tellg();
        midi_file_stream.read (memblock, sz);
        midi_file_stream.seekg (0, ios::end);
        end =  midi_file_stream.tellg();
        midi_file_stream.close();

        this->file_size = (end-begin);

        for(int i=0; i < this->file_size; i++){
            this->file_data.push_back(memblock[i]);
        }

        delete[] memblock;
        this->is_file_success_readed = true;
        return 1;
    } else {
        this->is_file_success_readed = false;
        return 0;
    }
}

int MidiFile::_find_chunks_and_decode(){
    this->fptr = 0;
    if( this->file_data[this->fptr++] == 77 &&
        this->file_data[this->fptr++] == 84 &&
        this->file_data[this->fptr++] == 104 &&
        this->file_data[this->fptr++] == 100 ){
        cout << "MThd ID OK" << endl;
    } else {
        return 0;
    }

    this->mthd_chunk_length  = (unsigned char)this->file_data[this->fptr++] << 24;
    this->mthd_chunk_length |= (unsigned char)this->file_data[this->fptr++] << 16;
    this->mthd_chunk_length |= (unsigned char)this->file_data[this->fptr++] << 8;
    this->mthd_chunk_length |= (unsigned char)this->file_data[this->fptr++];

    cout << "MThd Chunk Length: " << dec << this->mthd_chunk_length << endl;

    this->format_type  = (unsigned char)this->file_data[this->fptr++] << 8;
    this->format_type |= (unsigned char)this->file_data[this->fptr++];
    cout << "Format type: " << dec << this->format_type << endl;

    this->mtrk_chunks_cnt  = (unsigned char)this->file_data[this->fptr++] << 8;
    this->mtrk_chunks_cnt |= (unsigned char)this->file_data[this->fptr++];
    cout << "MTrk chunks cnt: " << dec << this->mtrk_chunks_cnt << endl;

    this->ppqn  = (unsigned char)this->file_data[this->fptr++] << 8;
    this->ppqn |= (unsigned char)this->file_data[this->fptr++];
    cout << "PPQN: " << dec << this->ppqn << endl;


    MTrkChunk *MTrk = new MTrkChunk[this->mtrk_chunks_cnt];

    for( int i=0; i < this->mtrk_chunks_cnt; i++){

        if( i == 0){
            if( MTrk[i].setInitData( this->fptr, this->file_data, this->octave_shift, this->note_shift ) == 1){
                cout << "MTrk chunk #" << i << " found\n";
            } else {
                cout << " @ERROR: MTrk chunk NOT #" << i << " found\n";
            }
        } else {
            if( MTrk[i].setInitData( MTrk[i-1].getEndPos(), this->file_data, this->octave_shift, this->note_shift ) == 1 ){
                cout << "MTrk chunk #" << i << " found\n";
            } else {
                cout << " @ERROR: MTrk chunk NOT #" << i << " found\n";
            }
        }


        if( MTrk[i].getTempo() > 0 ){
            this->tempto_track = MTrk[i].getTempo();
        }

        if( this->new_bpm > 0 ){
            this->tempto_track = this->new_bpm;
        }

        //cout << "NEW BPM: " << this->new_bpm << endl;
        cout << "BPM: " << this->tempto_track << endl;

        //cout << "Track BPM: " << this->tempto_track << endl;
        cout << "Notes ON Count in MTrk chunk: " << MTrk[i].getEventsNoteONCount() << endl;
        cout << "Notes OFF Count in MTrk chunk: " << MTrk[i].getEventsNoteOFFCount() << endl;
        //cout << "Track Name: " << MTrk[i].getTrackName() << endl;
        cout << "Track Text: " << MTrk[i].getTrackText() << endl;
        cout << "Note Channel: " << MTrk[i].getNoteChannel() << endl;

        /*
        cout << "Track Name: " << MTrk[i].getTrackName() << endl;
        cout << "Notes ON Count: " << MTrk[i].getEventsNoteONCount() << endl;
        cout << "Notes OFF Count: " << MTrk[i].getEventsNoteOFFCount() << endl;
        */

        int note_cnt_difference =  MTrk[i].getEventsNoteONCount() - MTrk[i].getEventsNoteOFFCount();

        if( note_cnt_difference == 0 && MTrk[i].getEventsNoteONCount() > 0 && MTrk[i].getEventsNoteOFFCount() > 0 ){
            this->pulses_per_sec = (double)60000 / ( this->tempto_track * this->ppqn );
            cout << "Pulses per second: " << this->pulses_per_sec << endl;

            this->freq_notes_arr = MTrk[i].getFreqNotes();
            this->length_notes_on_arr = MTrk[i].getLengthNotesON();
            this->length_notes_off_arr = MTrk[i].getLengthNotesOFF();
            this->symbolic_notes_arr = MTrk[i].getSymbolicNotes();
            //this->first_dly = MTrk[i].getFirstDelay();
            this->_create_mikrotik_script_file( i, MTrk[i].getTrackText(), MTrk[i].getTrackName(), MTrk[i].getEventsNoteONCount(), MTrk[i].getNoteChannel() );
        }

        cout << endl;
    }


    return 1;
}

int MidiFile::_create_mikrotik_script_file( int chunk_number, string track_text, string track_name_comment, int notes_cnt, int note_ch ){
    /*
    :beep frequency=440 length=1000ms;
    :delay 1000ms;
    */
    //cout << "WRITE FUNC RUN: " << chunk_number << endl;
    ostringstream ost_chunk_number;
    ost_chunk_number << chunk_number;

    ostringstream ost_bpm;
    ost_bpm << this->tempto_track;

    ostringstream ost_notes_cnt;
    ost_notes_cnt << notes_cnt;

    ostringstream ost_note_ch;
    ost_note_ch << note_ch;

    //ostringstream ost_first_dly;


    int status = 0;
    string spaces = "";
    string out_data = "";
    string out_path = this->file_path + ost_chunk_number.str() +".txt";;
    ofstream output_file;

    out_data += "#----------------File Description-----------------#\n";
    out_data += "# This file created by Midi To Mikrotik Converter\n";
    out_data += "# Original midi file name/path: " + this->file_path + "\n";
    out_data += "# Track BPM: " + ost_bpm.str() + "\n";
    out_data += "# MIDI Channel: " + ost_note_ch.str() + "\n";
    out_data += "# Number of notes: " + ost_notes_cnt.str() + "\n";
    out_data += "# Track text: " + track_text + "\n";
    out_data += "# Track comment: " + track_name_comment + "\n";
    out_data += "#-------------------------------------------------#\n\n";

    //out_data += ":delay " + ost_st_dly.str() + "ms;\n\n";

    /*
    double first_dly_double = 0;
    for( int j=0; j<this->first_dly.size(); j++){
        cout << " DBG: " << this->first_dly[j] << endl;
        first_dly_double += ( this->first_dly[j] * this->pulses_per_sec );
    }
    ost_first_dly << first_dly_double;
    out_data += ":delay " + ost_first_dly.str() + "ms;\n\n";
    */

    for(int i=0; i<this->freq_notes_arr.size(); i++){
        string comment_str = "";

        ostringstream ost_freq;
        ostringstream ost_note_duration_ms;
        ostringstream ost_pause_duration_ms;

        ost_freq << this->freq_notes_arr[i];

        //double note_on_duration_ms = this->length_notes_on_arr[i] * this->pulses_per_sec;

        double note_on_duration_ms = 0;
        for( int j=0; j<this->length_notes_on_arr[i].size(); j++){
            note_on_duration_ms += ( this->length_notes_on_arr[i][j] * this->pulses_per_sec );
        }

        //double pause_duration_ms = (this->length_notes_off_arr[i] * this->pulses_per_sec) + note_on_duration_ms;
        double pause_duration_ms = note_on_duration_ms;
        for( int j=0; j<this->length_notes_off_arr[i].size(); j++){
            pause_duration_ms += (this->length_notes_off_arr[i][j] * this->pulses_per_sec);
        }

        this->full_track_length += note_on_duration_ms + pause_duration_ms;

        ost_note_duration_ms << note_on_duration_ms;
        ost_pause_duration_ms << pause_duration_ms;

        if( this->comments_flag == true ){
            comment_str = " # " + this->symbolic_notes_arr[i];
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
    if( this->full_track_length > 60 ){
        this->ftl_m = this->full_track_length / 60;

        if( this->full_track_length > 60 ){
            this->ftl_h = this->full_track_length / 3600;
        }
    }

    //cout << "Hours: " << this->ftl_h << "Minutes: " << this->ftl_m << "Seconds: " << this->ftl_s << endl;
}

string MidiFile::getPath(){
    return this->file_path;
}

int MidiFile::getFileSize(){
    return this->file_size;
}

vector<unsigned char> MidiFile::getFileData(){
    return this->file_data;
}

double MidiFile::getFullTrackLength(){
    return this->full_track_length;
}
