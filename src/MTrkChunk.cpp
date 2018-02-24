#include "MTrkChunk.h"

MTrkChunk::MTrkChunk()
{
    //ctor
}

MTrkChunk::~MTrkChunk()
{
    //dtor
}

int MTrkChunk::setInitData( const int &chunk_ptr, const std::vector<unsigned char> &chunk_data, const int &_mtrk_octave_shift, const int &_mtrk_note_shift )
{
    m_chunkPtr = chunk_ptr;

    m_chunkData = chunk_data;
    m_mtrkStartPos = chunk_ptr;
    m_mtrkOctaveShift = _mtrk_octave_shift * 12;
    m_mtrkNoteShift = _mtrk_note_shift;


    if( _work() == 1 ){
        _detect_events_note_on();
        _detect_events_note_off();
        _detectTempo();
        _detectTrackName();
        _detect_text();
        _found_note_channel();
        //_detect_first_delay(); // -- not working, in process

        if(m_noteChannel >=0 && m_noteChannel <=16){
            std::cout << "Founded notes channel: " << m_noteChannel << std::endl;
        }

        //cout << "   dbg on note_ch: " << 144 + m_noteChannel << endl;
        //cout << "   dbg off note_ch: " << 128 + m_noteChannel << endl;

        return 1;
    } else {
        return 0;
    }
}

int MTrkChunk::_work(){

    if( m_chunkData[m_chunkPtr++] == 77 &&
        m_chunkData[m_chunkPtr++] == 84 &&
        m_chunkData[m_chunkPtr++] == 114 &&
        m_chunkData[m_chunkPtr++] == 107 ){

        //cout << "  DBG: MTrk chunk found" << endl;

        m_mtrkSize  = (unsigned char)m_chunkData[m_chunkPtr++] << 24;
        m_mtrkSize |= (unsigned char)m_chunkData[m_chunkPtr++] << 16;
        m_mtrkSize |= (unsigned char)m_chunkData[m_chunkPtr++] << 8;
        m_mtrkSize |= (unsigned char)m_chunkData[m_chunkPtr++];

        std::cout << "MTrk chunk length: " << std::dec << static_cast<int>(m_mtrkSize) << std::endl;
        m_mtrkEndPos = m_mtrkStartPos + m_mtrkSize + 8;


        for( int i=m_chunkPtr; i<m_chunkPtr + static_cast<int>(m_mtrkSize); i++){
            m_chunkBody.push_back(m_chunkData[i]);
        }
        return 1;
    } else {
        return 0;
    }
}

int MTrkChunk::getStartPos(){
    return m_mtrkStartPos;
}

int MTrkChunk::getEndPos(){
    return m_mtrkEndPos;
}

int MTrkChunk::getSize(){
    return m_mtrkSize;
}

void MTrkChunk::dbg_printBody(){
    int iter = 1;
    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        std::cout << static_cast<int>(m_chunkBody[i]) << "\t";
        //cout << std::hex << m_chunkBody[i] << "\t";

        if(iter == 4){
            std::cout << std::endl;
            iter = 0;
        }
        iter++;
    }
    std::cout << std::endl;
}

/*
void MTrkChunk::_detect_first_delay(){
    //dbg_printBody();

    bool loop_flag = true;
    int i = 0;
    int shift_1 = 1;
    int new_shift = 0;



    for( i=0; i<chunk_body.size(); i++){
        if( chunk_body[i] == 255 ){
            int header_shift = i + shift_1 + 2;
            if(chunk_body[i+shift_1] == 1) {new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 2) {new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 3) {new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 4) {new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 5) {new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 6) {new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 7) {new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 32){new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 33){new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 81){new_shift = (unsigned char)chunk_body[i + shift_1 + 1] + header_shift;}
            else if(chunk_body[i+shift_1] == 47){break;}
        }
        else if( i >= first_note_on_byte_addr ){
            break;
        }
        //i++;
        if( new_shift > 0 ){
            i += new_shift + 1;
        }
        cout << " I " << i << endl;
    }

    cout << " First note byte addr: " << first_note_on_byte_addr << endl;
    cout << " DBG: " << static_cast<int>(chunk_body[first_note_on_byte_addr]) << endl;
    cout << " NEW SHIFT ADDR: " << new_shift << endl;
    cout << " DBG BYTE BEFORE 0x90: "<< static_cast<int>(chunk_body[new_shift]) << endl;


    int counter = 0;
    int bytes_cnt = first_note_on_byte_addr - new_shift;

    for( int i=new_shift; i < first_note_on_byte_addr; i++){
        m_firstDelay.push_back(chunk_body[i]);
        counter++;
    }
    //cout << " DLY: " << static_cast<int>(m_firstDelay) << endl;
}
*/

/*
vector<int> MTrkChunk::getFirstDelay(){
    return m_firstDelay;
}
*/

int MTrkChunk::_found_note_channel(){
    m_noteChannel = 0;
    for(int i=0; i<17; i++){
        _detect_events_note_on();
        _detect_events_note_off();

        if( m_eventsNoteOn.size() > 0 && m_eventsNoteOff.size() > 0 && m_eventsNoteOn.size() == m_eventsNoteOff.size() ){
            //cout << "------------" << i << endl;
            return i;
        }

        //m_noteChannel = m_noteChannel + 1;
        m_noteChannel++;
    }
    return -1;
}

int MTrkChunk::getNoteChannel(){
    if( m_noteChannel > 16 ){
        return -1;
    } else {
        return m_noteChannel;
    }
}

int MTrkChunk::_detect_events_note_on(){
    int j=0;

    m_eventsNoteOn.clear();
    m_chunkLengthNotesOn.clear();
    m_chunkFreqNotes.clear();
    m_chunkSymbolicNotes.clear();

    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        if( m_chunkBody[i]   == (m_cmdNoteOn + m_noteChannel) && //144 - 0x90
            m_chunkBody[i+1] >= 0 && m_chunkBody[i+1] <= 127 &&
            m_chunkBody[i+2] >= 0 && m_chunkBody[i+2] <= 127 ){

            //find bytes before 0x8Y Y - channel number

            uint64_t k=0;
            int ptr_shift = 3;
            std::vector<int> note_on_len_multibyte_arr;

            for( k=0; k<m_chunkBody.size(); k++){
                if( m_chunkBody[i+ptr_shift+k] == (m_cmdNoteOff + m_noteChannel) ){
                    break;
                } else {
                    note_on_len_multibyte_arr.push_back( m_chunkBody[i+ptr_shift+k] );
                    //std::cout << " Last added note: " << <m_chunkBody[i+ptr_shift+k] << endl;
                }
            }
            //std::cout << " NOTE CH: " << m_noteChannel << endl;
            //std::cout << " CNT BYTES: " << note_on_len_multibyte_arr.size() << endl;





            std::vector<unsigned char> note;

            note.push_back(m_chunkBody[i]);    //0x90 - note on channel #1
            note.push_back(m_chunkBody[i+1]);  //note pitch
            note.push_back(m_chunkBody[i+2]);  //note velocity
            note.push_back(m_chunkBody[i+3]);  //note length


            m_eventsNoteOn.push_back(note);

            uint64_t note_index = m_chunkBody[i+1] + m_mtrkOctaveShift + m_mtrkNoteShift;

            if( note_index < 0 ){
                while( note_index < 0) note_index += 12;
            } else if( note_index > (m_freqNotes.size() -1) ){
                while( note_index > (m_freqNotes.size() - 1) ) note_index -= 12;
            }

            m_chunkFreqNotes.push_back(m_freqNotes[note_index]);
            m_chunkSymbolicNotes.push_back(m_symbolicNotes[note_index]);
            //chunk_length_notes_on.push_back(m_chunkBody[i+3]);
            m_chunkLengthNotesOn.push_back(note_on_len_multibyte_arr);

            //detecting addr of first note on event
            if( m_chunkFreqNotes.size() == 1 ){
                m_firstNoteOnByteAddr = i;
            }

            j++;
        }
    }

    if( j > 0 ){
        return 1;
    } else {
        return 0;
    }

    std::cout << std::endl;
}

std::vector<double> MTrkChunk::getFreqNotes(){
    return m_chunkFreqNotes;
}

std::vector<std::vector<int>> MTrkChunk::getLengthNotesON(){
    return m_chunkLengthNotesOn;
}

int MTrkChunk::getEventsNoteONCount(){
    return m_eventsNoteOn.size();
}

std::vector<std::string> MTrkChunk::getSymbolicNotes(){
    return m_chunkSymbolicNotes;
}

int MTrkChunk::_detect_events_note_off(){
    int j=0;

    m_eventsNoteOff.clear();
    m_chunkLengthNotesOff.clear();


    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        if( m_chunkBody[i] == (m_cmdNoteOff + m_noteChannel) && // 128 - 0x80
            m_chunkBody[i+1] >= 0 && m_chunkBody[i+1] <= 127 &&
            m_chunkBody[i+2] >= 0 && m_chunkBody[i+2] <= 127 ){


            //find bytes before FF 2F 00

            uint64_t k=0;
            int ptr_shift = 3;
            std::vector<int> note_off_len_multibyte_arr;

            for( k=0; k<m_chunkBody.size(); k++){
                if( m_chunkBody[i+ptr_shift+k] == (m_cmdNoteOn + m_noteChannel) ||
                    (m_chunkBody[i+ptr_shift+k] == 255 &&
                     m_chunkBody[i+ptr_shift+k+1] == 47 &&
                     m_chunkBody[i+ptr_shift+k+2] == 0) ){
                    break;
                } else {
                    note_off_len_multibyte_arr.push_back( m_chunkBody[i+ptr_shift+k] );
                }
            }


            std::vector<unsigned char> note;

            note.push_back(m_chunkBody[i]);    //0x80 - note off channel #1
            note.push_back(m_chunkBody[i+1]);  //note pitch
            note.push_back(m_chunkBody[i+2]);  //note velocity
            note.push_back(m_chunkBody[i+3]);  //note length

            m_eventsNoteOff.push_back(note);
            //chunk_length_notes_off.push_back(m_chunkBody[i+3]);
            m_chunkLengthNotesOff.push_back(note_off_len_multibyte_arr);

            j++;
        }
    }

    if( j > 0 ){
        return 1;
    } else {
        return 0;
    }
}

std::vector<std::vector<int>> MTrkChunk::getLengthNotesOFF(){
    return m_chunkLengthNotesOff;
}

int MTrkChunk::getEventsNoteOFFCount(){
    return m_eventsNoteOff.size();
}

void MTrkChunk::dbg_printSymbolicNotes(){
    if( m_eventsNoteOn.size() > 0 ){
        for( uint64_t i=0; i<m_eventsNoteOn.size(); i++){
            for( int j=0; j<4; j++){
                if( j == 1){
                    std::cout << m_symbolicNotes[static_cast<int>(m_eventsNoteOn[i][j])];
                }
            }
            std::cout << std::endl;
        }
    }
}

void MTrkChunk::dbg_printFreqNotes(){
    if( m_eventsNoteOn.size() > 0 ){
        for( uint64_t i=0; i<m_eventsNoteOn.size(); i++){
            for( int j=0; j<4; j++){
                if( j == 1){
                    std::cout << m_freqNotes[static_cast<int>(m_eventsNoteOn[i][j])];
                }
            }
            std::cout << std::endl;
        }
    }
}


int MTrkChunk::_detectTempo(){
    //FF 51 03 tt tt tt
    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        if( m_chunkBody[i]   == 255 &&
            m_chunkBody[i+1] == 81 &&
            m_chunkBody[i+2] == 3 ){

            int tmp_tempo = 0;
            tmp_tempo  = (unsigned char)m_chunkBody[i+3] << 16;
            tmp_tempo |= (unsigned char)m_chunkBody[i+4] << 8;
            tmp_tempo |= (unsigned char)m_chunkBody[i+5];
            m_tempo = 60000000 / tmp_tempo;

            //cout << " Tempo: " << m_tempo << " BPM" << endl;
        }
    }
}

int MTrkChunk::getTempo(){
    return m_tempo;
}

int MTrkChunk::_detectTrackName(){
    //FF 03 len text
    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        if( m_chunkBody[i]   == 255 &&
            m_chunkBody[i+1] == 3 ){

            m_trackNameLength = m_chunkBody[i+2];

            for( int j=i+3; j<i+3+m_trackNameLength; j++){
                m_trackName.push_back(m_chunkBody[j]);
            }
        }
    }
}

int MTrkChunk::getTrackNameLength(){
    return m_trackNameLength;
}

std::string MTrkChunk::getTrackName(){
    return m_trackName;
}

void MTrkChunk::_detect_text(){
    //FF 01 len text
    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        if( m_chunkBody[i]   == 255 &&
            m_chunkBody[i+1] == 1 ){

            m_trackTextLength = m_chunkBody[i+2];

            for( int j=i+3; j<i+3+m_trackTextLength; j++){
                m_trackText.push_back(m_chunkBody[j]);
            }
        }
    }
}

int MTrkChunk::getTrackTextLength(){
    return m_trackTextLength;
}

std::string MTrkChunk::getTrackText(){
    return m_trackText;
}

