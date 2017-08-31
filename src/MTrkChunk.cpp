#include "MTrkChunk.h"

MTrkChunk::MTrkChunk()
{
    //ctor
}

MTrkChunk::~MTrkChunk()
{
    //dtor
}

int MTrkChunk::setInitData( int chunk_ptr, vector<unsigned char> chunk_data, int _mtrk_octave_shift, int _mtrk_note_shift ){
    cptr = chunk_ptr;

    cdata = chunk_data;
    this->mtrk_start_pos = chunk_ptr;
    this->mtrk_octave_shift = _mtrk_octave_shift * 12;
    this->mtrk_note_shift = _mtrk_note_shift;


    if( this->_work() == 1 ){
        this->_detect_events_note_on();
        this->_detect_events_note_off();
        this->_detectTempo();
        this->_detectTrackName();
        this->_detect_text();
        this->_found_note_channel();
        //this->_detect_first_delay(); // -- not working, in process

        if(this->note_channel >=0 && this->note_channel <=16){
            cout << "Founded notes channel: " << this->note_channel << endl;
        }

        //cout << "   dbg on note_ch: " << 144 + this->note_channel << endl;
        //cout << "   dbg off note_ch: " << 128 + this->note_channel << endl;

        return 1;
    } else {
        return 0;
    }
}

int MTrkChunk::_work(){

    if( this->cdata[this->cptr++] == 77 &&
        this->cdata[this->cptr++] == 84 &&
        this->cdata[this->cptr++] == 114 &&
        this->cdata[this->cptr++] == 107 ){

        //cout << "  DBG: MTrk chunk found" << endl;

        this->mtrk_sz  = (unsigned char)this->cdata[this->cptr++] << 24;
        this->mtrk_sz |= (unsigned char)this->cdata[this->cptr++] << 16;
        this->mtrk_sz |= (unsigned char)this->cdata[this->cptr++] << 8;
        this->mtrk_sz |= (unsigned char)this->cdata[this->cptr++];

        cout << "MTrk chunk length: " << std::dec << static_cast<int>(this->mtrk_sz) << endl;
        this->mtrk_end_pos = this->mtrk_start_pos + this->mtrk_sz + 8;


        for( int i=this->cptr; i<this->cptr + static_cast<int>(this->mtrk_sz); i++){
            this->chunk_body.push_back(this->cdata[i]);
        }
        return 1;
    } else {
        return 0;
    }
}

int MTrkChunk::getStartPos(){
    return this->mtrk_start_pos;
}

int MTrkChunk::getEndPos(){
    return this->mtrk_end_pos;
}

int MTrkChunk::getSize(){
    return this->mtrk_sz;
}

void MTrkChunk::dbg_printBody(){
    int iter = 1;
    for( int i=0; i<this->chunk_body.size(); i++){
        cout << static_cast<int>(this->chunk_body[i]) << "\t";
        //cout << std::hex << this->chunk_body[i] << "\t";

        if(iter == 4){
            cout << endl;
            iter = 0;
        }
        iter++;
    }
    cout << endl;
}

/*
void MTrkChunk::_detect_first_delay(){
    //this->dbg_printBody();

    bool loop_flag = true;
    int i = 0;
    int shift_1 = 1;
    int new_shift = 0;



    for( i=0; i<this->chunk_body.size(); i++){
        if( this->chunk_body[i] == 255 ){
            int header_shift = i + shift_1 + 2;
            if(this->chunk_body[i+shift_1] == 1) {new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 2) {new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 3) {new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 4) {new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 5) {new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 6) {new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 7) {new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 32){new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 33){new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 81){new_shift = (unsigned char)this->chunk_body[i + shift_1 + 1] + header_shift;}
            else if(this->chunk_body[i+shift_1] == 47){break;}
        }
        else if( i >= this->first_note_on_byte_addr ){
            break;
        }
        //i++;
        if( new_shift > 0 ){
            i += new_shift + 1;
        }
        cout << " I " << i << endl;
    }

    cout << " First note byte addr: " << this->first_note_on_byte_addr << endl;
    cout << " DBG: " << static_cast<int>(this->chunk_body[this->first_note_on_byte_addr]) << endl;
    cout << " NEW SHIFT ADDR: " << new_shift << endl;
    cout << " DBG BYTE BEFORE 0x90: "<< static_cast<int>(this->chunk_body[new_shift]) << endl;


    int counter = 0;
    int bytes_cnt = this->first_note_on_byte_addr - new_shift;

    for( int i=new_shift; i < this->first_note_on_byte_addr; i++){
        this->first_delay.push_back(this->chunk_body[i]);
        counter++;
    }
    //cout << " DLY: " << static_cast<int>(this->first_delay) << endl;
}
*/

/*
vector<int> MTrkChunk::getFirstDelay(){
    return this->first_delay;
}
*/

int MTrkChunk::_found_note_channel(){
    this->note_channel = 0;
    for(int i=0; i<17; i++){
        this->_detect_events_note_on();
        this->_detect_events_note_off();

        if( this->events_note_on.size() > 0 && this->events_note_off.size() > 0 && this->events_note_on.size() == this->events_note_off.size() ){
            //cout << "------------" << i << endl;
            return i;
        }

        //this->note_channel = this->note_channel + 1;
        this->note_channel++;
    }
    return -1;
}

int MTrkChunk::getNoteChannel(){
    if( this->note_channel > 16 ){
        return -1;
    } else {
        return this->note_channel;
    }
}

int MTrkChunk::_detect_events_note_on(){
    int j=0;

    this->events_note_on.clear();
    this->chunk_length_notes_on.clear();
    this->chunk_freq_notes.clear();
    this->chunk_symbolic_notes.clear();

    for( int i=0; i<this->chunk_body.size(); i++){
        if( this->chunk_body[i]   == (this->cmd_note_on + this->note_channel) && //144 - 0x90
            this->chunk_body[i+1] >= 0 && this->chunk_body[i+1] <= 127 &&
            this->chunk_body[i+2] >= 0 && this->chunk_body[i+2] <= 127 ){

            //find bytes before 0x8Y Y - channel number

            int k=0;
            int ptr_shift = 3;
            vector<int> note_on_len_multibyte_arr;

            for( k=0; k<this->chunk_body.size(); k++){
                if( this->chunk_body[i+ptr_shift+k] == (this->cmd_note_off + this->note_channel) ){
                    break;
                } else {
                    note_on_len_multibyte_arr.push_back( this->chunk_body[i+ptr_shift+k] );
                    //cout << " Last added note: " << <this->chunk_body[i+ptr_shift+k] << endl;
                }
            }
            //cout << " NOTE CH: " << this->note_channel << endl;
            //cout << " CNT BYTES: " << note_on_len_multibyte_arr.size() << endl;





            vector<unsigned char> note;

            note.push_back(this->chunk_body[i]);    //0x90 - note on channel #1
            note.push_back(this->chunk_body[i+1]);  //note pitch
            note.push_back(this->chunk_body[i+2]);  //note velocity
            note.push_back(this->chunk_body[i+3]);  //note length


            this->events_note_on.push_back(note);

            int note_index = this->chunk_body[i+1] + this->mtrk_octave_shift + this->mtrk_note_shift;

            if( note_index < 0 ){
                while( note_index < 0) note_index += 12;
            } else if( note_index > (this->freq_notes.size() -1) ){
                while( note_index > (this->freq_notes.size() - 1) ) note_index -= 12;
            }

            this->chunk_freq_notes.push_back(this->freq_notes[note_index]);
            this->chunk_symbolic_notes.push_back(this->symbolic_notes[note_index]);
            //this->chunk_length_notes_on.push_back(this->chunk_body[i+3]);
            this->chunk_length_notes_on.push_back(note_on_len_multibyte_arr);

            //detecting addr of first note on event
            if( this->chunk_freq_notes.size() == 1 ){
                this->first_note_on_byte_addr = i;
            }

            j++;
        }
    }

    if( j > 0 ){
        return 1;
    } else {
        return 0;
    }

    cout << endl;
}

vector<double> MTrkChunk::getFreqNotes(){
    return this->chunk_freq_notes;
}

vector<vector<int>> MTrkChunk::getLengthNotesON(){
    return this->chunk_length_notes_on;
}

int MTrkChunk::getEventsNoteONCount(){
    return this->events_note_on.size();
}

vector<string> MTrkChunk::getSymbolicNotes(){
    return this->chunk_symbolic_notes;
}

int MTrkChunk::_detect_events_note_off(){
    int j=0;

    this->events_note_off.clear();
    this->chunk_length_notes_off.clear();


    for( int i=0; i<this->chunk_body.size(); i++){
        if( this->chunk_body[i] == (this->cmd_note_off + this->note_channel) && // 128 - 0x80
            this->chunk_body[i+1] >= 0 && this->chunk_body[i+1] <= 127 &&
            this->chunk_body[i+2] >= 0 && this->chunk_body[i+2] <= 127 ){


            //find bytes before FF 2F 00

            int k=0;
            int ptr_shift = 3;
            vector<int> note_off_len_multibyte_arr;

            for( k=0; k<this->chunk_body.size(); k++){
                if( this->chunk_body[i+ptr_shift+k] == (this->cmd_note_on + this->note_channel) ||
                    (this->chunk_body[i+ptr_shift+k] == 255 &&
                     this->chunk_body[i+ptr_shift+k+1] == 47 &&
                     this->chunk_body[i+ptr_shift+k+2] == 0) ){
                    break;
                } else {
                    note_off_len_multibyte_arr.push_back( this->chunk_body[i+ptr_shift+k] );
                }
            }


            vector<unsigned char> note;

            note.push_back(this->chunk_body[i]);    //0x80 - note off channel #1
            note.push_back(this->chunk_body[i+1]);  //note pitch
            note.push_back(this->chunk_body[i+2]);  //note velocity
            note.push_back(this->chunk_body[i+3]);  //note length

            this->events_note_off.push_back(note);
            //this->chunk_length_notes_off.push_back(this->chunk_body[i+3]);
            this->chunk_length_notes_off.push_back(note_off_len_multibyte_arr);

            j++;
        }
    }

    if( j > 0 ){
        return 1;
    } else {
        return 0;
    }
}

vector<vector<int>> MTrkChunk::getLengthNotesOFF(){
    return this->chunk_length_notes_off;
}

int MTrkChunk::getEventsNoteOFFCount(){
    return this->events_note_off.size();
}

void MTrkChunk::dbg_printSymbolicNotes(){
    if( this->events_note_on.size() > 0 ){
        for( int i=0; i<this->events_note_on.size(); i++){
            for( int j=0; j<4; j++){
                if( j == 1){
                    cout << this->symbolic_notes[static_cast<int>(this->events_note_on[i][j])];
                }
            }
            cout << endl;
        }
    }
}

void MTrkChunk::dbg_printFreqNotes(){
    if( this->events_note_on.size() > 0 ){
        for( int i=0; i<this->events_note_on.size(); i++){
            for( int j=0; j<4; j++){
                if( j == 1){
                    cout << this->freq_notes[static_cast<int>(this->events_note_on[i][j])];
                }
            }
            cout << endl;
        }
    }
}


int MTrkChunk::_detectTempo(){
    //FF 51 03 tt tt tt
    for( int i=0; i<this->chunk_body.size(); i++){
        if( this->chunk_body[i]   == 255 &&
            this->chunk_body[i+1] == 81 &&
            this->chunk_body[i+2] == 3 ){

            int tmp_tempo = 0;
            tmp_tempo  = (unsigned char)this->chunk_body[i+3] << 16;
            tmp_tempo |= (unsigned char)this->chunk_body[i+4] << 8;
            tmp_tempo |= (unsigned char)this->chunk_body[i+5];
            this->tempo = 60000000 / tmp_tempo;

            //cout << " Tempo: " << this->tempo << " BPM" << endl;
        }
    }
}

int MTrkChunk::getTempo(){
    return this->tempo;
}

int MTrkChunk::_detectTrackName(){
    //FF 03 len text
    for( int i=0; i<this->chunk_body.size(); i++){
        if( this->chunk_body[i]   == 255 &&
            this->chunk_body[i+1] == 3 ){

            this->track_name_length = this->chunk_body[i+2];

            for( int j=i+3; j<i+3+this->track_name_length; j++){
                this->track_name.push_back(chunk_body[j]);
            }
        }
    }
}

int MTrkChunk::getTrackNameLength(){
    return this->track_name_length;
}

string MTrkChunk::getTrackName(){
    return this->track_name;
}

void MTrkChunk::_detect_text(){
    //FF 01 len text
    for( int i=0; i<this->chunk_body.size(); i++){
        if( this->chunk_body[i]   == 255 &&
            this->chunk_body[i+1] == 1 ){

            this->track_text_length = this->chunk_body[i+2];

            for( int j=i+3; j<i+3+this->track_text_length; j++){
                this->track_text.push_back(chunk_body[j]);
            }
        }
    }
}

int MTrkChunk::getTrackTextLength(){
    return this->track_text_length;
}

string MTrkChunk::getTrackText(){
    return this->track_text;
}

