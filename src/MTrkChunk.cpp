#include "../include/MTrkChunk.h"

const static std::vector<std::string> m_symbolicNotes = {
    "C 0", "C# 0", "D 0", "Eb 0", "E 0", "F 0", "F# 0", "G 0", "G# 0", "A 0", "Bb 0", "B 0",  /* #0 */
    "C 1", "C# 1", "D 1", "Eb 1", "E 1", "F 1", "F# 1", "G 1", "G# 1", "A 1", "Bb 1", "B 1",  /* #1 */
    "C 2", "C# 2", "D 2", "Eb 2", "E 2", "F 2", "F# 2", "G 2", "G# 2", "A 2", "Bb 2", "B 2",  /* #2 */
    "C 3", "C# 3", "D 3", "Eb 3", "E 3", "F 3", "F# 3", "G 3", "G# 3", "A 3", "Bb 3", "B 3",  /* #3 */
    "C 4", "C# 4", "D 4", "Eb 4", "E 4", "F 4", "F# 4", "G 4", "G# 4", "A 4", "Bb 4", "B 4",  /* #4 */
    "C 5", "C# 5", "D 5", "Eb 5", "E 5", "F 5", "F# 5", "G 5", "G# 5", "A 5", "Bb 5", "B 5",  /* #5 */
    "C 6", "C# 6", "D 6", "Eb 6", "E 6", "F 6", "F# 6", "G 6", "G# 6", "A 6", "Bb 6", "B 6",  /* #6 */
    "C 7", "C# 7", "D 7", "Eb 7", "E 7", "F 7", "F# 7", "G 7", "G# 7", "A 7", "Bb 7", "B 7",  /* #7 */
    "C 8", "C# 8", "D 8", "Eb 8", "E 8", "F 8", "F# 8", "G 8", "G# 8", "A 8", "Bb 8", "B 8",  /* #8 */
    "C 9", "C# 9", "D 9", "Eb 9", "E 9", "F 9", "F# 9", "G 9", "G# 9", "A 9", "Bb 9", "B 9"}; /* #9 */

const static std::vector<double> m_freqNotes = {
    8.18,    8.66,    9.18,    9.72,    10.30,   10.91,     11.56,    12.25,   12.98,   13.75,  14.57,   15.43,    /* #0 */
    16.35,   17.32,   18.35,   19.45,   20.60,   21.83,     23.12,    24.50,   25.96,   27.50,  29.14,   30.87,    /* #1 */
    32.70,   34.65,   36.71,   38.89,   41.20,   43.65,     46.25,    49.0,    51.91,   55.0,   58.27,   61.74,    /* #2 */
    65.41,   69.30,   73.42,   77.78,   82.41,   87.31,     92.50,    98.0,    103.83,  110.0,  116.54,  123.47,   /* #3 */
    130.81,  138.59,  146.83,  155.56,  164.81,  174.61,    185.0,    196.0,   207.65,  220.0,  233.08,  246.94,   /* #4 */
    261.63,  277.18,  293.66,  311.13,  329.63,  349.23,    369.99,   392.0,   415.30,  440.0,  466.16,  493.88,   /* #5 */
    523.25,  554.37,  587.33,  622.25,  659.25,  698.46,    739.99,   783.99,  830.61,  880.0,  932.33,  987.77,   /* #6 */
    1046.5,  1108.73, 1174.66, 1244.51, 1318.51, 1396.91,   1479.98,  1567.98, 1661.22, 1760.0, 1864.66, 1975.53,  /* #7 */
    2093.0,  2217.46, 2349.32, 2489.02, 2637.02, 2793.83,   2959.96,  3135.96, 3322.44, 3520.0, 3729.31, 3951.07,  /* #8 */
    4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65,   5919.91,  6271.93, 6644.88, 7040.0, 7458.62, 7902.13}; /* #9 */

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


    if( m_work() == 1 ){
        m_detect_events_note_on();
        m_detect_events_note_off();
        m_detectTempo();
        m_detectTrackName();
        m_detect_text();
        m_found_note_channel();
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

int MTrkChunk::m_work(){

    if( m_chunkData[m_chunkPtr++] == 0x4D &&    // 0x4D - 77  - M
        m_chunkData[m_chunkPtr++] == 0x54 &&    // 0x54 - 84  - T
        m_chunkData[m_chunkPtr++] == 0x72 &&    // 0x72 - 114 - r
        m_chunkData[m_chunkPtr++] == 0x6B )     // 0x6B - 107 - k
    {

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

int MTrkChunk::m_found_note_channel(){
    m_noteChannel = 0;
    for(int i=0; i<17; i++){
        m_detect_events_note_on();
        m_detect_events_note_off();

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

int MTrkChunk::m_detect_events_note_on(){
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

int MTrkChunk::m_detect_events_note_off(){
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


int MTrkChunk::m_detectTempo(){
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
    return m_tempo;
}

int MTrkChunk::getTempo(){
    return m_tempo;
}

std::string MTrkChunk::m_detectTrackName(){
    //FF 03 len text
    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        if( m_chunkBody[i]   == 255 &&
            m_chunkBody[i+1] == 3 ){

            m_trackNameLength = m_chunkBody[i+2];

            for( uint64_t j=i+3; j<i+3+m_trackNameLength; j++){
                m_trackName.push_back(m_chunkBody[j]);
            }
        }
    }
    return m_trackName;
}

int MTrkChunk::getTrackNameLength(){
    return m_trackNameLength;
}

std::string MTrkChunk::getTrackName(){
    return m_trackName;
}

void MTrkChunk::m_detect_text(){
    //FF 01 len text
    for( uint64_t i=0; i<m_chunkBody.size(); i++){
        if( m_chunkBody[i]   == 255 &&
            m_chunkBody[i+1] == 1 ){

            m_trackTextLength = m_chunkBody[i+2];

            for( uint64_t j=i+3; j<i+3+m_trackTextLength; j++){
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

