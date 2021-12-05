#include "Note.hpp"
#include <boost/log/trivial.hpp>

const static uint8_t NOTES_IN_OCTAVE = 12;

const static std::vector<std::string> g_symbolicNotes = {
	"C 0", "C# 0", "D 0", "Eb 0", "E 0", "F 0", "F# 0", "G 0", "G# 0", "A 0", "Bb 0", "B 0",  /* #0 */
	"C 1", "C# 1", "D 1", "Eb 1", "E 1", "F 1", "F# 1", "G 1", "G# 1", "A 1", "Bb 1", "B 1",  /* #1 */
	"C 2", "C# 2", "D 2", "Eb 2", "E 2", "F 2", "F# 2", "G 2", "G# 2", "A 2", "Bb 2", "B 2",  /* #2 */
	"C 3", "C# 3", "D 3", "Eb 3", "E 3", "F 3", "F# 3", "G 3", "G# 3", "A 3", "Bb 3", "B 3",  /* #3 */
	"C 4", "C# 4", "D 4", "Eb 4", "E 4", "F 4", "F# 4", "G 4", "G# 4", "A 4", "Bb 4", "B 4",  /* #4 */
	"C 5", "C# 5", "D 5", "Eb 5", "E 5", "F 5", "F# 5", "G 5", "G# 5", "A 5", "Bb 5", "B 5",  /* #5 */
	"C 6", "C# 6", "D 6", "Eb 6", "E 6", "F 6", "F# 6", "G 6", "G# 6", "A 6", "Bb 6", "B 6",  /* #6 */
	"C 7", "C# 7", "D 7", "Eb 7", "E 7", "F 7", "F# 7", "G 7", "G# 7", "A 7", "Bb 7", "B 7",  /* #7 */
	"C 8", "C# 8", "D 8", "Eb 8", "E 8", "F 8", "F# 8", "G 8", "G# 8", "A 8", "Bb 8", "B 8",  /* #8 */
	"C 9", "C# 9", "D 9", "Eb 9", "E 9", "F 9", "F# 9", "G 9", "G# 9", "A 9", "Bb 9", "B 9"   /* #9 */
};

const static std::vector<double> g_freqNotes = {
	8.18,    8.66,    9.18,    9.72,    10.30,   10.91,     11.56,    12.25,   12.98,   13.75,  14.57,   15.43,    /* #0 */
	16.35,   17.32,   18.35,   19.45,   20.60,   21.83,     23.12,    24.50,   25.96,   27.50,  29.14,   30.87,    /* #1 */
	32.70,   34.65,   36.71,   38.89,   41.20,   43.65,     46.25,    49.0,    51.91,   55.0,   58.27,   61.74,    /* #2 */
	65.41,   69.30,   73.42,   77.78,   82.41,   87.31,     92.50,    98.0,    103.83,  110.0,  116.54,  123.47,   /* #3 */
	130.81,  138.59,  146.83,  155.56,  164.81,  174.61,    185.0,    196.0,   207.65,  220.0,  233.08,  246.94,   /* #4 */
	261.63,  277.18,  293.66,  311.13,  329.63,  349.23,    369.99,   392.0,   415.30,  440.0,  466.16,  493.88,   /* #5 */
	523.25,  554.37,  587.33,  622.25,  659.25,  698.46,    739.99,   783.99,  830.61,  880.0,  932.33,  987.77,   /* #6 */
	1046.5,  1108.73, 1174.66, 1244.51, 1318.51, 1396.91,   1479.98,  1567.98, 1661.22, 1760.0, 1864.66, 1975.53,  /* #7 */
	2093.0,  2217.46, 2349.32, 2489.02, 2637.02, 2793.83,   2959.96,  3135.96, 3322.44, 3520.0, 3729.31, 3951.07,  /* #8 */
	4186.01, 4434.92, 4698.63, 4978.03, 5274.04, 5587.65,   5919.91,  6271.93, 6644.88, 7040.0, 7458.62, 7902.13   /* #9 */
};

Note::Note(Event &event)
{
	MidiEventCode cmd = event.getCmd();
	m_cmd = cmd.getMainCmd();
	m_channel = cmd.getSubCmd();

	std::vector<uint8_t> data = event.getData();
	if(data.size() != 2)
		return;
	m_pitch = data[0];
	m_velocity = data[1];
	m_delay = event.getDelay();
}

void Note::log()
{
	BOOST_LOG_TRIVIAL(info) << "Note cmd: " << (uint32_t)m_cmd
	<< " channel: " << (uint32_t)m_channel
	<< " pitch: " << (uint32_t)m_pitch
	<< " velocity: " << (uint32_t)m_velocity
	<< " duration value: " << (uint32_t)m_delay.getValue();
}

NOTE_TYPE Note::getType()
{
	return (NOTE_TYPE)m_cmd;
}

double Note::getFrequencyHz(
	const int octaveShift, 
	const int noteShift, 
	const double fineTuning
)
{
	return g_freqNotes[m_pitch + (octaveShift * NOTES_IN_OCTAVE) + noteShift] + fineTuning;
}

std::string Note::getSymbolicNote(
	const int octaveShift, 
	const int noteShift, 
	const double fineTuning
)
{
	std::string symbolic = g_symbolicNotes[m_pitch + (octaveShift * NOTES_IN_OCTAVE) + noteShift];
	if(fineTuning != 0.0)
	{
		symbolic += std::string(" ");
		symbolic += ((fineTuning < 0.0) ? "-" : "+");
		symbolic += fineTuning;
		symbolic += std::string(" Hz");
	}
	return symbolic;
}

VLV Note::getDelay()
{
	return m_delay;
}
