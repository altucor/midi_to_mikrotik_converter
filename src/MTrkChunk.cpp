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

void MTrkChunk::initialize(std::vector<uint8_t> &fileData, uint64_t startPos, uint64_t endPos)
{
    std::vector<uint8_t>::iterator begIter = fileData.begin() + startPos;
    std::vector<uint8_t>::iterator endIter = fileData.begin() + endPos;
    m_chunkData = std::vector<uint8_t>(begIter, endIter);

    m_mtrkStartPos = startPos;
    m_mtrkEndPos = endPos;
    m_mtrkSize = endPos - startPos;
}

void MTrkChunk::setOctaveShift(int octaveShift)
{
    m_mtrkOctaveShift = octaveShift * 12;
}

void MTrkChunk::setNoteShift(int noteShift)
{
    m_mtrkNoteShift = noteShift;
}

void MTrkChunk::setDebugLevel(int debugLevel)
{
    m_debugLevel = debugLevel;
}

int64_t MTrkChunk::getStartPos()
{
    return m_mtrkStartPos;
}

int64_t MTrkChunk::getEndPos()
{
    return m_mtrkEndPos;
}

int64_t MTrkChunk::getSize()
{
    return m_mtrkSize;
}

int MTrkChunk::process()
{
    if( m_work() == 0 ){
        m_stateMachine();
        //m_detectNoteEvents(m_cmdNoteOnMask, m_notesOn);
        //m_detectNoteEvents(m_cmdNoteOffMask, m_notesOff);
        //m_mapEventsToHumanizedNotes();
        //m_detectTempo();
        //m_detectTrackName();
        //m_detectTrackText();
        //_detect_first_delay(); // -- not working, in process
        return 0;
    } else {
        return -1;
    }
}

void MTrkChunk::m_stateMachine()
{
    STATE machineState = UNDEFINED;
    uint64_t chunkPtr = 0;

    while(true)
    {
        switch(m_cms)
        {
            case UNDEFINED:
            {
                if( m_chunkData[chunkPtr++] == 0x4D &&    // 0x4D - 77  - M
                    m_chunkData[chunkPtr++] == 0x54 &&    // 0x54 - 84  - T
                    m_chunkData[chunkPtr++] == 0x72 &&    // 0x72 - 114 - r
                    m_chunkData[chunkPtr++] == 0x6B )     // 0x6B - 107 - k
                {
                    m_cms = HEADER_MARKER;
                    if(m_debugLevel >= 4)
                        std::cout << "Found MTrk header marker" << std::endl;
                }
                break;
            }

            case HEADER_MARKER:
            {
                m_mtrkSize  = m_chunkData[chunkPtr++] << 24;
                m_mtrkSize |= m_chunkData[chunkPtr++] << 16;
                m_mtrkSize |= m_chunkData[chunkPtr++] << 8;
                m_mtrkSize |= m_chunkData[chunkPtr++];
                m_mtrkSize += 8; // Include 4 bytes of MTrk header and 4 bytes of MTrk chunk size

                if(m_debugLevel >= 4)
                    std::cout << "Found MTrk header size and first delay" << std::endl;

                m_cms = FIRST_DELAY;
                break;
            }

            case UNKNOWN_BYTE:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found UNKNOWN_BYTE" << std::endl;
                break;
            }

            case FIRST_DELAY:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found FIRST_DELAY" << std::endl;

                m_firstDelays.push_back(m_chunkData[chunkPtr++]);

                uint8_t currentCmd = m_chunkData[chunkPtr++];

                if(currentCmd == 0xFF){
                    m_cms = EVENT; break;
                } else if((currentCmd >> 4) == m_cmdNoteOnMask){
                    m_cms = NOTE_ON; break;
                } else if((currentCmd >> 4) == m_cmdNoteOffMask){
                    m_cms = NOTE_OFF; break;
                } else {
					m_cms = FIRST_DELAY; chunkPtr--; break;
                }
            }

            case EVENT:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found EVENT" << std::endl;

                uint8_t currentCmd = m_chunkData[chunkPtr++];

                if(currentCmd == 0x01){
                    m_cms = TEXT; chunkPtr++; break;
                } else if(currentCmd == 0x02){
                    m_cms = COPYRIGHT; chunkPtr++; break;
                } else if(currentCmd == 0x03){
                    m_cms = TRACK_NAME; break;
                } else if(currentCmd == 0x04){
                    m_cms = INSTRUMENT_NAME; chunkPtr++; break;
                } else if(currentCmd == 0x05){
                    m_cms = VOCAL_TEXT; chunkPtr++; break;
                } else if(currentCmd == 0x06){
                    m_cms = TEXT_MARKER; chunkPtr++; break;
                } else if(currentCmd == 0x07){
                    m_cms = CUE_POINT; chunkPtr++; break;
                } else if(currentCmd == 0x20 && m_chunkData[chunkPtr] == 0x01){
                    m_cms = MIDI_CHANNEL; chunkPtr++; break;
                } else if(currentCmd == 0x21 && m_chunkData[chunkPtr] == 0x01){
                    m_cms = MIDI_PORT; chunkPtr++; break;
                } else if(currentCmd == 0x51 && m_chunkData[chunkPtr] == 0x03){
					m_cms = TEMPO; chunkPtr++; break;
                } else if(currentCmd == 0x54 && m_chunkData[chunkPtr] == 0x05){
                    m_cms = SMPTE_OFFSET; chunkPtr++; break;
                } else if(currentCmd == 0x58 && m_chunkData[chunkPtr] == 0x04){
                    m_cms = TIME_SIGNATURE; chunkPtr++; break;
                } else if(currentCmd == 0x59 && m_chunkData[chunkPtr] == 0x02){
                    m_cms = KEY_SIGNATURE; chunkPtr++; break;
                } else if(currentCmd == 0x2F && m_chunkData[chunkPtr] == 0x00){
                    m_cms = TRACK_END; chunkPtr++; break;
                } else {
                    m_cms = EVENT;
                    break;
                }
            }

            case UNPREDICTED:
            {
                std::cout << " --- >> MTrk STATE MACHINE: Unpredicted Behaviour" << std::endl;
                return;
            }
            case TEXT:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found TEXT event" << std::endl;

                break;
            }
            case COPYRIGHT:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found COPYRIGHT event" << std::endl;

                break;
            }
            case TRACK_NAME:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found TRACK_NAME event" << std::endl;

				
				uint8_t trackNameSize = m_chunkData[chunkPtr++];
				uint32_t currentPos = chunkPtr;
				while (chunkPtr != currentPos + trackNameSize)
				{
					m_trackName.push_back(m_chunkData[chunkPtr++]);
				}

				m_cms = FIRST_DELAY;
                break;
            }
            case INSTRUMENT_NAME:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found INSTRUMENT_NAME event" << std::endl;

                break;
            }
            case VOCAL_TEXT:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found VOCAL_TEXT event" << std::endl;

                break;
            }
            case TEXT_MARKER:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found TEXT_MARKER event" << std::endl;

                break;
            }
            case CUE_POINT:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found CUE_POINT event" << std::endl;

                break;
            }
            case MIDI_CHANNEL:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found MIDI_CHANNEL event" << std::endl;

                break;
            }
            case MIDI_PORT:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found MIDI_PORT event" << std::endl;

                break;
            }
            case TEMPO:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found TEMPO event" << std::endl;

				int32_t tmp_tempo = 0;
				tmp_tempo = m_chunkData[chunkPtr++] << 16;
				tmp_tempo |= m_chunkData[chunkPtr++] << 8;
				tmp_tempo |= m_chunkData[chunkPtr++];
				m_tempo = 60000000 / tmp_tempo;

				m_cms = FIRST_DELAY;
                break;
            }
            case SMPTE_OFFSET:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found SMPTE_OFFSET event" << std::endl;

                break;
            }
            case TIME_SIGNATURE:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found TIME_SIGNATURE event" << std::endl;

				m_timeSignature.nn = m_chunkData[chunkPtr++];
				m_timeSignature.dd = m_chunkData[chunkPtr++];
				m_timeSignature.cc = m_chunkData[chunkPtr++];
				m_timeSignature.bb = m_chunkData[chunkPtr++];

				m_cms = FIRST_DELAY;
                break;
            }
            case KEY_SIGNATURE:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found KEY_SIGNATURE event" << std::endl;

                break;
            }
            case TRACK_END:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found TRACK_END event" << std::endl;

				return;
            }
            case NOTE_ON:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found NOTE_ON event" << std::endl;

				chunkPtr--;
				NoteEvent onEvent;
				onEvent.noteCmd.cmd = (m_chunkData[chunkPtr] >> 4);
				onEvent.noteCmd.channel = (m_chunkData[chunkPtr] & 0x0F);
				chunkPtr++;
				onEvent.pitch = m_chunkData[chunkPtr++];
				onEvent.velocity = m_chunkData[chunkPtr++];

				if (m_channel == 0xFF)
					m_channel = onEvent.noteCmd.channel;

				while(!m_isNoteEnd(chunkPtr))
					onEvent.length.push_back(m_chunkData[chunkPtr++]);

				m_notesOn.push_back(onEvent);

				uint8_t nextByte = m_chunkData[chunkPtr++];
				if ((nextByte >> 4) == m_cmdNoteOffMask)
					m_cms = NOTE_OFF;
				else if ((nextByte >> 4) == m_cmdNoteOnMask)
					m_cms = NOTE_ON;
				else if (nextByte == eventMarker)
					m_cms = TRACK_END;

                break;
            }
            case NOTE_OFF:
            {
                if(m_debugLevel >= 4)
                    std::cout << "Found NOTE_OFF event" << std::endl;

				chunkPtr--;
				NoteEvent offEvent;
				offEvent.noteCmd.cmd = (m_chunkData[chunkPtr] >> 4);
				offEvent.noteCmd.channel = (m_chunkData[chunkPtr] & 0x0F);
				chunkPtr++;
				offEvent.pitch = m_chunkData[chunkPtr++];
				offEvent.velocity = m_chunkData[chunkPtr++];

				if (m_channel == 0xFF)
					m_channel = offEvent.noteCmd.channel;

				while (!m_isNoteEnd(chunkPtr))
					offEvent.length.push_back(m_chunkData[chunkPtr++]);

				m_notesOff.push_back(offEvent);

				uint8_t nextByte = m_chunkData[chunkPtr++];
				if ((nextByte >> 4) == m_cmdNoteOffMask)
					m_cms = NOTE_OFF;
				else if ((nextByte >> 4) == m_cmdNoteOnMask)
					m_cms = NOTE_ON;
				else if (nextByte == eventMarker)
					m_cms = TRACK_END;

				break;
            }
        }
    }
}

int MTrkChunk::m_work(){

    if( m_chunkData[m_chunkPtr++] == 0x4D &&    // 0x4D - 77  - M
        m_chunkData[m_chunkPtr++] == 0x54 &&    // 0x54 - 84  - T
        m_chunkData[m_chunkPtr++] == 0x72 &&    // 0x72 - 114 - r
        m_chunkData[m_chunkPtr++] == 0x6B )     // 0x6B - 107 - k
    {
        std::cout << "MTrk chunk length: " << std::dec << static_cast<int>(m_mtrkSize) << std::endl;
        return 0;
    } else {
        return -1;
    }
}

int MTrkChunk::m_getChannelFromCmd(uint8_t cmd)
{
    return cmd && 0x0F;
}

void MTrkChunk::m_debugPrintNoteEvent(NoteEvent note)
{
    std::cout << "Cmd: "       << std::to_string(note.noteCmd.cmd);
    std::cout << "\tChannel: "   << std::to_string(note.noteCmd.channel);
    std::cout << "\tPitch: "     << std::to_string(note.pitch);
    std::cout << "\tVelocity: "  << std::to_string(note.velocity);
    std::cout << "\tLength size: "    << std::to_string(note.length.size()) << std::endl;
}

bool MTrkChunk::m_isTrackEnd(uint64_t startIndex)
{
    if(m_chunkData[startIndex] == 0xFF &&
       m_chunkData[startIndex + 1] == 0x2F &&
       m_chunkData[startIndex + 2] == 0x00)
        return true;
    else
        return false;
}

bool MTrkChunk::m_isNoteEnd(uint64_t startIndex)
{
	//m_channel
	if (m_chunkData[startIndex] == eventMarker)
		return true;

    if((m_chunkData[startIndex] >> 4) == m_cmdNoteOnMask || (m_chunkData[startIndex] >> 4) == m_cmdNoteOffMask)
		if((m_chunkData[startIndex] & 0x0F) == m_channel)
			return true;

	return false;
}

int MTrkChunk::m_detectNoteEvents(uint8_t cmdMask, std::vector<NoteEvent> &noteEvents)
{
    for(uint64_t i=0; i < m_chunkData.size(); i++)
    {
        if((m_chunkData[i] >> 4) == cmdMask &&
            m_chunkData[i+1] >= 0 && m_chunkData[i+1] <= 127 &&
            m_chunkData[i+2] >= 0 && m_chunkData[i+2] <= 127)
        {
            //const NoteEvent *currentNoteEvent = reinterpret_cast<const NoteEvent*>(m_chunkData.data() + i);
            NoteEvent currentNoteEvent;
            currentNoteEvent.noteCmd.cmd = (m_chunkData[i] >> 4);
            currentNoteEvent.noteCmd.channel = (m_chunkData[i] & 0x0F);
            currentNoteEvent.pitch = m_chunkData[i+1];
            currentNoteEvent.velocity = m_chunkData[i+2];
            currentNoteEvent.length.push_back(m_chunkData[i+3]);

            uint32_t additionalNoteLengthIter = 0;
            while(!m_isNoteEnd(i + 4 + additionalNoteLengthIter))
            {
                currentNoteEvent.length.push_back(m_chunkData[i + 4 + additionalNoteLengthIter]);
                std::cout << "Pushing byte: " << std::to_string(m_chunkData[i + 4 + additionalNoteLengthIter]) << std::endl;
                additionalNoteLengthIter++;
            }

            if(m_debugLevel >= 2)
                m_debugPrintNoteEvent(currentNoteEvent);

            noteEvents.push_back(currentNoteEvent);
        }
    }

    if(m_debugLevel >= 1)
        std::cout << "Found " << noteEvents.size() << " note events with cmd " << std::to_string(cmdMask) << std::endl;

    return 0;
}

void MTrkChunk::m_debugPrintHumanizedNote(HumanizedNote hNote)
{
    std::cout << "Symbolic: " << hNote.symbolicCode;
    std::cout << "\tFrequency: " << hNote.frequency;
    std::cout << "\tCount of duration On bytes: " << hNote.durationOn.size();
    std::cout << "\tCount of duration Off bytes: " << hNote.durationOff.size() << std::endl;
}

void MTrkChunk::m_mapEventsToHumanizedNotes()
{
    for(uint64_t i = 0; i < m_notesOn.size(); i++)
    {
        uint64_t noteIndex = m_notesOn[i].pitch + m_mtrkOctaveShift + m_mtrkNoteShift;

        if( noteIndex < 0 )
            while( noteIndex < 0) noteIndex += 12;
        else if( noteIndex > (m_freqNotes.size() -1) )
            while( noteIndex > (m_freqNotes.size() - 1) ) noteIndex -= 12;

        HumanizedNote hNote;
        hNote.symbolicCode = m_symbolicNotes[noteIndex];
        hNote.frequency = m_freqNotes[noteIndex];
        hNote.durationOn = m_notesOn[i].length;
        hNote.durationOff = m_notesOff[i].length;

        if(m_debugLevel >= 1)
            m_debugPrintHumanizedNote(hNote);

        m_humanizedNotes.push_back(hNote);
    }
}

int8_t MTrkChunk::getMidiChannel()
{
    if(m_notesOn.size() == 0)
        return -1;

    return m_notesOn[0].noteCmd.channel;
}

int32_t MTrkChunk::getNotesCount()
{
    if(m_notesOn.size() != m_notesOff.size())
        return -1;

    return m_notesOn.size();
}

std::vector<HumanizedNote> MTrkChunk::getHumanizedNotes()
{
    return m_humanizedNotes;
}

void MTrkChunk::dbg_printBody()
{
    std::cout << " - Debug Chunk Data:" << std::endl;

    int iter = 1;
    for( uint64_t i=0; i<m_chunkData.size(); i++){
        std::cout << static_cast<int>(m_chunkData[i]) << "\t";
        //cout << std::hex << m_chunkData[i] << "\t";

        if(iter == 4){
            std::cout << std::endl;
            iter = 0;
        }
        iter++;
    }
    std::cout << std::endl;
}

void MTrkChunk::m_detectTempo()
{
    //FF 51 03 tt tt tt
    for( uint64_t i=0; i<m_chunkData.size(); i++)
    {
        if( m_chunkData[i]   == 0xFF &&
            m_chunkData[i+1] == 0x51 &&
            m_chunkData[i+2] == 0x03 ){

            int32_t tmp_tempo = 0;
            tmp_tempo  = m_chunkData[i+3] << 16;
            tmp_tempo |= m_chunkData[i+4] << 8;
            tmp_tempo |= m_chunkData[i+5];
            m_tempo = 60000000 / tmp_tempo;

            if(m_debugLevel >= 2)
            std::cout << "Detected track tempo: " << m_tempo << " BPM" << std::endl;
        }
    }
}

int MTrkChunk::getTempo()
{
    return m_tempo;
}

void MTrkChunk::m_detectTrackName()
{
    //FF 03 len text
    int trackNameLength = 0;
    for( uint64_t i=0; i<m_chunkData.size(); i++)
    {
        if( m_chunkData[i]   == 0xFF &&
            m_chunkData[i+1] == 0x03 ){

            trackNameLength = m_chunkData[i+2];

            for( uint64_t j=i+3; j<i+3+trackNameLength; j++){
                m_trackName.push_back(m_chunkData[j]);
            }
        }
    }
}

std::string MTrkChunk::getTrackName()
{
    return m_trackName;
}

void MTrkChunk::m_detectTrackText()
{
    //FF 01 len text
    int trackTextLength = 0;
    for( uint64_t i=0; i<m_chunkData.size(); i++)
    {
        if( m_chunkData[i]   == 0xFF &&
            m_chunkData[i+1] == 0x01 ){

            trackTextLength = m_chunkData[i+2];

            for( uint64_t j=i+3; j<i+3+trackTextLength; j++){
                m_trackText.push_back(m_chunkData[j]);
            }
        }
    }
}

std::string MTrkChunk::getTrackText()
{
    return m_trackText;
}
