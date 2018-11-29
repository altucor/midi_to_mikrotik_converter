#include "../include/MidiFile.h"

/*
MThd - 77 84 104 100
MTrk - 77 84 114 107
*/

const static uint8_t CONTINUATION_BIT = 0x80;

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
    m_tempoTrack = m_newBpm;
}

void MidiFile::setDebugLevel(int debugLevel)
{
    m_debugLevel = debugLevel;
}

void MidiFile::setEnableCommentsTo(bool enableCommentsFlag)
{
    m_commentsFlag = enableCommentsFlag;
}

void MidiFile::setPredelayFlag(bool predelayFlag)
{
    m_predelayFlag = predelayFlag;
}


void MidiFile::parseFile()
{
    if( m_readFile() == 0){
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

    if(m_findMtrkChunks() == 0){
        std::cout << "Success found MTrk chunks" << std::endl;
    } else {
        std::cout << " @ERROR: cannot found any chunks" << std::endl;
        exit(-1);
    }

    if(m_processChunks() == 0){
        std::cout << "Success processed MTrk chunks" << std::endl;
    } else {
        std::cout << " @ERROR: cannot process chunks" << std::endl;
        exit(-1);
    }


    if(m_generateOutputTrackFiles() == 0){
        std::cout << "Success generated output scripts" << std::endl;
    } else {
        std::cout << " @ERROR: while generating script" << std::endl;
        exit(-1);
    }

}

int MidiFile::m_readFile()
{
    if(m_filePath == "")
        return -1;

    std::ifstream midiFileStream(m_filePath, std::ios::binary);
	if (!midiFileStream.is_open())
		return -1;

	midiFileStream.seekg(0, std::ios::end);
	m_fileData.resize(midiFileStream.tellg());
	midiFileStream.seekg(0, std::ios::beg);
	midiFileStream.read(reinterpret_cast<char*>(m_fileData.data()), m_fileData.size());
	midiFileStream.close();

    return 0;
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

    m_mthdChunkLength  = m_fileData[m_filePtr++] << 24;
    m_mthdChunkLength |= m_fileData[m_filePtr++] << 16;
    m_mthdChunkLength |= m_fileData[m_filePtr++] << 8;
    m_mthdChunkLength |= m_fileData[m_filePtr++];

    std::cout << "MThd Chunk Length: " << std::dec << m_mthdChunkLength << std::endl;

    m_formatType  = m_fileData[m_filePtr++] << 8;
    m_formatType |= m_fileData[m_filePtr++];
    std::cout << "Format type: " << std::dec << m_formatType << std::endl;

    m_mtrkChunksCnt  = m_fileData[m_filePtr++] << 8;
    m_mtrkChunksCnt |= m_fileData[m_filePtr++];
    std::cout << "MTrk chunks cnt: " << std::dec << m_mtrkChunksCnt << std::endl;

    m_ppqn  = m_fileData[m_filePtr++] << 8;
    m_ppqn |= m_fileData[m_filePtr++];
    std::cout << "PPQN: " << std::dec << m_ppqn << std::endl;

    if(m_formatType != 0 && m_formatType != 1)
    {
        std::cout << " @ERROR: supported only midi files with type 0 or 1." << std::endl;
        return -1;
    }

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

        chunkInfo.size  = m_fileData[m_filePtr++] << 24;
        chunkInfo.size |= m_fileData[m_filePtr++] << 16;
        chunkInfo.size |= m_fileData[m_filePtr++] << 8;
        chunkInfo.size |= m_fileData[m_filePtr++];
        chunkInfo.size += 8; // We include 4 bytes of "MTrk" header and 4 bytes which contains chunk size
        chunkInfo.endPos = chunkInfo.startPos + chunkInfo.size;
        m_filePtr = chunkInfo.endPos;
        chunkInfo.chunkNumber = i;

        if(m_debugLevel >= 1)
        {
            std::cout << " - Chunk header: " << chunkInfo.mtrk << std::endl;
            std::cout << " - Chunk start pos: " << chunkInfo.startPos << std::endl;
            std::cout << " - Chunk end pos: " << chunkInfo.endPos << std::endl;
            std::cout << " - Chunk size: " << chunkInfo.size << std::endl << std::endl;
        }

        m_mtrkChunks.push_back(chunkInfo);
    }

    return 0;
}

int MidiFile::m_processChunks()
{
    for(auto & chunk : m_mtrkChunks)
    {
        if(m_processChunk(chunk) != 0)
            return -1;
    }
     return 0;
}

int MidiFile::m_processChunk(MtrkChunkInfo &chunkInfo)
{
    chunkInfo.mtrkChunkHandler = MTrkChunk();
    chunkInfo.mtrkChunkHandler.setOctaveShift(m_octaveShift);
    chunkInfo.mtrkChunkHandler.setNoteShift(m_noteShift);
    chunkInfo.mtrkChunkHandler.initialize(m_fileData, chunkInfo.startPos, chunkInfo.endPos);
    chunkInfo.mtrkChunkHandler.setDebugLevel(m_debugLevel);

    if(m_debugLevel >= 3)
        std::cout << "Processing chunk, start: " << chunkInfo.startPos << " end: " << chunkInfo.endPos << std::endl;

    if(chunkInfo.mtrkChunkHandler.process() == 0)
    {
        uint32_t tempoTrack = chunkInfo.mtrkChunkHandler.getTempo();
        std::string trackText = chunkInfo.mtrkChunkHandler.getTrackText();
        std::string trackName = chunkInfo.mtrkChunkHandler.getTrackName();

        if(tempoTrack != 0 && m_newBpm == -1)
        {
            m_tempoTrack = tempoTrack;
            std::cout << "BPM: " << std::to_string(m_tempoTrack) << std::endl;
        }

        if(trackText != "")
        {
            m_trackText = trackText;
            std::cout << "Track text: " << m_trackText << std::endl;
        }

        if(trackName != "")
        {
            m_trackName = trackName;
            std::cout << "Track name: " << m_trackName << std::endl;
        }

        m_pulsesPerSec = (double)60000 / ( m_tempoTrack * m_ppqn );
        std::cout << "Pulses per second: " << m_pulsesPerSec << std::endl;

        if(m_debugLevel >= 5)
            chunkInfo.mtrkChunkHandler.dbg_printBody();

        std::cout << "OK Processed chunk" << std::endl;
    }
    else
    {
        std::cout << " @ERROR Processing chunk" << std::endl;
    }

    return 0;
}

int MidiFile::m_generateOutputTrackFiles()
{
    for(int64_t i = 0; i < m_mtrkChunks.size(); i++)
    {
        if(m_createMikrotikScriptFile(m_mtrkChunks[i]) != 0)
            return -1;
    }

    return 0;
}

double MidiFile::m_durationArrayToMiliseconds(std::vector<uint8_t> delayEvents)
{
	double result = 0.0;
	uint32_t assembledValue = 0;
	uint64_t eventsTotal = delayEvents.size() - 1;

	/*
	 * Very detailed article about VLV(Variable Length Values)
	 * http://www.ccarh.org/courses/253/handout/vlv/
	 * Thats why 0x7F and -1 at the end
	 */
	for (auto & item : delayEvents)
	{
		uint8_t skipContinuationBit = 0;

		if (item & CONTINUATION_BIT)
			skipContinuationBit = 1;
		assembledValue |= (item & 0x7F) << ((eventsTotal * 8) - skipContinuationBit);
		eventsTotal--;
	}

	result = (double)(static_cast<double>(assembledValue) * m_pulsesPerSec);

	return result;
}

int MidiFile::m_createMikrotikScriptFile(MtrkChunkInfo chunk)
{
    /*
     * :beep frequency=440 length=1000ms;
     * :delay 1000ms;
     */

    int status = 0;
    std::stringstream outFilePath;
    outFilePath << m_filePath << chunk.chunkNumber << ".txt";
    std::stringstream outputBuffer;
    int8_t midiChannel = chunk.mtrkChunkHandler.getMidiChannel();
    int64_t notesCount = chunk.mtrkChunkHandler.getNotesCount();
    std::vector<HumanizedNote> humanizedNotes = chunk.mtrkChunkHandler.getHumanizedNotes();

    if(midiChannel == -1 || notesCount == 0 || notesCount == -1 || humanizedNotes.size() == 0)
    {
        std::cout << "Skipping MTrk #" << chunk.chunkNumber << " no information about notes" << std::endl;
        return 0;
    }

	// here calculated first pre-delay
	double totalFirstDelay = m_calculateFirstDelay(chunk.mtrkChunkHandler.getFirstDelays());
    double totalTrackSizeInMs = totalFirstDelay + m_calculateFullTrackSize(chunk);
    TrackLength ctl = m_converTimeToReadable(totalTrackSizeInMs); // ctl - current track length

    outputBuffer << "#----------------File Description-----------------#\n";
    outputBuffer << "# This file generated by Midi To Mikrotik Converter\n";
    outputBuffer << "# Original midi file name/path: " << m_filePath << "\n";
    outputBuffer << "# Track BPM: " << m_tempoTrack << "\n";
    outputBuffer << "# MIDI Channel: " << std::to_string(midiChannel) << "\n";
    outputBuffer << "# Number of notes: " << std::to_string(notesCount) << "\n";
    outputBuffer << "# Track length: " << ctl.h << "h " << ctl.m << "m " << ctl.s << "s " << ctl.ms << "ms\n";
    outputBuffer << "# Track text: " << chunk.mtrkChunkHandler.getTrackText() << "\n";
    outputBuffer << "# Track copyright: " << chunk.mtrkChunkHandler.getCopyright() << "\n";
    outputBuffer << "# Track name: " << chunk.mtrkChunkHandler.getTrackName() << "\n";
    outputBuffer << "# Vocals: " << chunk.mtrkChunkHandler.getInstrumentName() << "\n";
    outputBuffer << "# Instrument name: " << chunk.mtrkChunkHandler.getVocalText() << "\n";
    outputBuffer << "# Text marker: " << chunk.mtrkChunkHandler.getTextMarker() << "\n";
    outputBuffer << "# Cue Point: " << chunk.mtrkChunkHandler.getCuePoint() << "\n";
    outputBuffer << "#-------------------------------------------------#\n\n";


    /*
	   * * * Feature pre-delay / first delay * * *
     * Sometimes it works bad, because some files have strange bytes like delay events,
     * but really they aren't detected as pre-delay by other programs.
     * So, if you really know, pre-delay is present before first note,
     * you can enable this feature by specifying flag "-p"
     *
     * You need this feature, only if you want play simultaneously several instruments on each router
     * and want to hear start of all tracks/instruments at the right time
     */
    if(m_predelayFlag)
        outputBuffer << ":delay " << totalFirstDelay << "ms; First delay / Pre-delay\n\n";

    for(const auto & note : humanizedNotes)
    {
        double noteOnDuration = 0.0;
        double noteOffDuration = 0.0;

		noteOnDuration = m_durationArrayToMiliseconds(note.durationOn);
		noteOffDuration = m_durationArrayToMiliseconds(note.durationOff);

        outputBuffer << ":beep frequency=" << note.frequency;
        outputBuffer << " length=" << noteOnDuration << "ms;";
        if(m_commentsFlag)
            outputBuffer << " # " << note.symbolicCode;
        outputBuffer << "\n:delay " << (noteOffDuration + noteOnDuration) << "ms;\n\n";
    }

    std::ofstream outputFile;
    outputFile.open(outFilePath.str());
    if(outputFile.is_open()){
        status = 0;
    } else {
        status = -1;
    }
    outputFile << outputBuffer.str();
    outputFile.close();

    std::cout << "Successfully writed MTrk #" << chunk.chunkNumber << std::endl;

    return status;
}

std::string MidiFile::getPath()
{
    return m_filePath;
}

int64_t MidiFile::getFileSize()
{
    return m_fileSize;
}

std::vector<uint8_t> MidiFile::getFileData()
{
    return m_fileData;
}

double MidiFile::m_calculateFullTrackSize(MtrkChunkInfo & chunkInfo)
{
    double trackDuration = 0.0;
    std::vector<HumanizedNote> humanizedNotes = chunkInfo.mtrkChunkHandler.getHumanizedNotes();

    for(const auto & note : humanizedNotes)
    {
        trackDuration += m_durationArrayToMiliseconds(note.durationOn);
        trackDuration += m_durationArrayToMiliseconds(note.durationOff);
    }
    return trackDuration;
}

TrackLength MidiFile::m_converTimeToReadable(double lengthInMs)
{
    TrackLength tl;
    tl.ms = (((int)lengthInMs%1000));
    tl.s  = (((int)lengthInMs/1000)%60);
    tl.m  = (((int)lengthInMs/(1000*60))%60);
    tl.h  = (((int)lengthInMs/(1000*60*60))%24);
    return tl;
}

double MidiFile::m_calculateFirstDelay(std::vector<uint8_t> firstDelays)
{
	bool appendVLV = false;
    double totalDelay = 0.0;
    uint32_t bufferForTicks = 0;

	// This array *firstDelays* contains all delay bytes which we found before first note event
    for(uint64_t i=0; i<firstDelays.size(); i++)
    {
		if ((firstDelays[i] & CONTINUATION_BIT)) // when we found continuation bit in the sequence
		{
			bufferForTicks = bufferForTicks << 7;
			bufferForTicks |= (firstDelays[i] & 0x7F);
			appendVLV = true;
		}
		else if (appendVLV && !(firstDelays[i] & CONTINUATION_BIT)) // when we appended already some bytes and it is the end of the sequence
		{
			bufferForTicks = bufferForTicks << 7;
			bufferForTicks |= (firstDelays[i] & 0x7F);

			if(bufferForTicks != 0)
				totalDelay += (double)(static_cast<double>(bufferForTicks) * m_pulsesPerSec);
			bufferForTicks = 0;
			appendVLV = false;
		}
		else if (!appendVLV && !(firstDelays[i] & CONTINUATION_BIT)) // just append solo byte
		{
			totalDelay += (double)(static_cast<double>(firstDelays[i]) * m_pulsesPerSec);
		}
    }

    return totalDelay;
}
