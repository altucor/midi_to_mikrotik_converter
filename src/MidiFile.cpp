#include "../include/MidiFile.h"

/*
MThd - 77 84 104 100
MTrk - 77 84 114 107
*/

MidiFile::MidiFile()
{
    //ctor
}

MidiFile::~MidiFile()
{
    //dtor
}

void MidiFile::setFilePath(std::string &filePath)
{
	m_filePath = filePath;
}

void MidiFile::setOctaveShift(int octaveShift)
{
    m_octaveShift = octaveShift;
}

void MidiFile::setNoteShift(int noteShift)
{
    m_noteShift = noteShift;
}

void MidiFile::setNewBpm(int bpm)
{
    m_newBpm = bpm;
}

void MidiFile::setEnableCommentsTo(bool enableCommentsFlag)
{
    m_commentsFlag = enableCommentsFlag;
}

void MidiFile::parseFile()
{
    if( m_readFile( m_filePath ) == 0){
        std::cout << "Success opened file" << std::endl;
    } else {
        std::cout << " @ERROR: file can not be opened" << std::endl;
        exit(-1);
    }

    if(m_parseHeader() == 0){
        std::cout << "Success detected MThd header" << std::endl;
    } else {
        std::cout << " @ERROR: cannot detect MThd header" << std::endl;
        exit(-1);
    }

    if( m_findChunksAndDecode() == 0){
        std::cout << "Standard Midi File structure detected" << std::endl;
    } else {
        std::cout << " @ERROR: This file does not have a SMF(Standart Midi File) structure " << std::endl;
        exit(-1);
    }

    m_humanizeTime();
}

int MidiFile::m_readFile( std::string path )
{
    std::ifstream midiFileStream( path, std::ios::in | std::ios::binary | std::ios::ate );
    midiFileStream.seekg(0, std::ios::end);
	m_fileSize = midiFileStream.tellg();
	m_fileData.resize(m_fileSize);
	midiFileStream.seekg(0, std::ios::beg);

    if (midiFileStream.is_open())
    {
        midiFileStream.read(reinterpret_cast<char*>(m_fileData.data()), m_fileSize);
    }

    if(m_fileSize == m_fileData.size())
    {
        m_fileSuccessReaded = true;
        return 0;
    }
    else
    {
        m_fileSuccessReaded = false;
        return -1;
    }
}

int MidiFile::m_parseHeader()
{
    m_filePtr = 0;
    if( m_fileData[m_filePtr++] == 0x4D &&   // M
		m_fileData[m_filePtr++] == 0x54 &&   // T
		m_fileData[m_filePtr++] == 0x68 &&   // h
		m_fileData[m_filePtr++] == 0x64 )    // d
    {
        std::cout << "MThd ID OK" << std::endl;
    } else {
        return -1;
    }

    m_mthdChunkLength  = reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 24;
    m_mthdChunkLength |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 16;
    m_mthdChunkLength |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 8;
    m_mthdChunkLength |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]);

    std::cout << "MThd Chunk Length: " << std::dec << m_mthdChunkLength << std::endl;

    format_type  = reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 8;
    format_type |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]);
    std::cout << "Format type: " << std::dec << format_type << std::endl;

    m_mtrkChunksCnt  = reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 8;
    m_mtrkChunksCnt |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]);
    std::cout << "MTrk chunks cnt: " << std::dec << m_mtrkChunksCnt << std::endl;

    m_ppqn  = reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 8;
    m_ppqn |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]);
    std::cout << "PPQN: " << std::dec << m_ppqn << std::endl;

    m_findMtrkChunks();

    return 0;
}

int MidiFile::m_findMtrkChunks()
{
    for(int i=0; i<m_mtrkChunksCnt; i++)
    {
        MtrkChunkInfo chunkInfo;
        chunkInfo.startPos = m_filePtr;

        chunkInfo.mtrk[0] = m_fileData[m_filePtr++];
        chunkInfo.mtrk[1] = m_fileData[m_filePtr++];
        chunkInfo.mtrk[2] = m_fileData[m_filePtr++];
        chunkInfo.mtrk[3] = m_fileData[m_filePtr++];

        chunkInfo.size  = reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 24;
        chunkInfo.size |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 16;
        chunkInfo.size |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]) << 8;
        chunkInfo.size |= reinterpret_cast<uint8_t>(m_fileData[m_filePtr++]);
        chunkInfo.endPos = chunkInfo.startPos + chunkInfo.size;
        m_filePtr = chunkInfo.endPos + 8; // We include 4 bytes of "MTrk" header and 4 bytes which contains chunk size

        std::cout << " - Chunk header: " << chunkInfo.mtrk << std::endl;
        std::cout << " - Chunk start pos: " << chunkInfo.startPos << std::endl;
        std::cout << " - Chunk end pos: " << chunkInfo.endPos << std::endl;
        std::cout << " - Chunk size: " << chunkInfo.size << std::endl;

        m_mtrkChunks.push_back(chunkInfo);
    }

    return 0;
}

int MidiFile::m_processChunk(MtrkChunkInfo chunkInfo)
{
    chunkInfo.mtrkChunkHandler = new MTrkChunk;
    chunkInfo.mtrkChunkHandler->setInitData();
    chunkInfo.mtrkChunkHandler->setOctaveShift(m_octaveShift);
    chunkInfo.mtrkChunkHandler->setNoteShift(m_noteShift);
    chunkInfo.mtrkChunkHandler->process();
}

int MidiFile::m_findChunksAndDecode()
{
    MTrkChunk *MTrk = new MTrkChunk[m_mtrkChunksCnt];

    for( int i=0; i < m_mtrkChunksCnt; i++)
    {

        if( i == 0){
            if( MTrk[i].setInitData( m_filePtr, m_fileData, m_octaveShift, m_noteShift ) == 1){
                std::cout << "MTrk chunk #" << i << " found\n";
            } else {
                std::cout << " @ERROR: MTrk chunk NOT #" << i << " found\n";
            }
        } else {
            if( MTrk[i].setInitData( MTrk[i-1].getEndPos(), m_fileData, m_octaveShift, m_noteShift ) == 1 ){
                std::cout << "MTrk chunk #" << i << " found\n";
            } else {
                std::cout << " @ERROR: MTrk chunk NOT #" << i << " found\n";
            }
        }


        if( MTrk[i].getTempo() > 0 ){
            m_tempoTrack = MTrk[i].getTempo();
        }

        if( m_newBpm > 0 ){
            m_tempoTrack = m_newBpm;
        }

        std::cout << "BPM: " << m_tempoTrack << std::endl;
        std::cout << "Notes ON Count in MTrk chunk: " << MTrk[i].getEventsNoteONCount() << std::endl;
        std::cout << "Notes OFF Count in MTrk chunk: " << MTrk[i].getEventsNoteOFFCount() << std::endl;
        std::cout << "Track Name: " << MTrk[i].getTrackName() << std::endl;
        std::cout << "Track Text: " << MTrk[i].getTrackText() << std::endl;
        std::cout << "Note Channel: " << MTrk[i].getNoteChannel() << std::endl;

        int note_cnt_difference =  MTrk[i].getEventsNoteONCount() - MTrk[i].getEventsNoteOFFCount();

        if( note_cnt_difference == 0 && MTrk[i].getEventsNoteONCount() > 0 && MTrk[i].getEventsNoteOFFCount() > 0 ){
            pulses_per_sec = (double)60000 / ( m_tempoTrack * m_ppqn );
            std::cout << "Pulses per second: " << pulses_per_sec << std::endl;

            freq_notes_arr = MTrk[i].getFreqNotes();
            length_notes_on_arr = MTrk[i].getLengthNotesON();
            length_notes_off_arr = MTrk[i].getLengthNotesOFF();
            symbolic_notes_arr = MTrk[i].getSymbolicNotes();
            //first_dly = MTrk[i].getFirstDelay();
			m_createMikrotikScriptFile( i, MTrk[i].getTrackText(), MTrk[i].getTrackName(), MTrk[i].getEventsNoteONCount(), MTrk[i].getNoteChannel() );
        }
        std::cout << std::endl;
    }
    return 0;
}

int MidiFile::m_createMikrotikScriptFile( const int &chunk_number, const std::string &track_text, const std::string &track_name_comment, const int &notes_cnt, const int &note_ch )
{
    /*
    :beep frequency=440 length=1000ms;
    :delay 1000ms;
    */
    //cout << "WRITE FUNC RUN: " << chunk_number << endl;
    std::ostringstream ost_chunk_number;
    ost_chunk_number << chunk_number;

    std::ostringstream ost_bpm;
    ost_bpm << m_tempoTrack;

    std::ostringstream ost_notes_cnt;
    ost_notes_cnt << notes_cnt;

    std::ostringstream ost_note_ch;
    ost_note_ch << note_ch;

    //ostringstream ost_first_dly;


    int status = 0;
    std::string spaces = "";
    std::string out_data = "";
    std::string out_path = m_filePath + ost_chunk_number.str() +".txt";;
    std::ofstream output_file;

    out_data += "#----------------File Description-----------------#\n";
    out_data += "# This file created by Midi To Mikrotik Converter\n";
    out_data += "# Original midi file name/path: " + m_filePath + "\n";
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

        if( m_commentsFlag == true ){
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

void MidiFile::m_humanizeTime()
{
    if( full_track_length > 60 ){
        ftl_m = full_track_length / 60;

        if( full_track_length > 60 ){
            ftl_h = full_track_length / 3600;
        }
    }

    //cout << "Hours: " << ftl_h << "Minutes: " << ftl_m << "Seconds: " << ftl_s << endl;
}

std::string MidiFile::getPath()
{
    return m_filePath;
}

int MidiFile::getFileSize()
{
    return m_fileSize;
}

std::vector<uint8_t> MidiFile::getFileData()
{
    return m_fileData;
}

double MidiFile::getFullTrackLength()
{
    return full_track_length;
}
