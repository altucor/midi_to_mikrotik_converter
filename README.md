# midi_to_mikrotik_converter

# Using programm

| Key | Description | Example |
| ------ | ------ | ------ |
| -f | Select input standart midi file (SMF) | -f <input_file.mid> |
| -o | Sets the octave shift relative to the original (-10 to +10) | -o -2 //Shifts all notes 2 octaves down |
| -n | Sets the note shift relative to the original | -n 3 //Shifts all notes 3 notes up |
| -b | Sets the new bpm to output file | -b 130 //Sets a new bpm to 130, ignores the original bpm in the file |
| -c | Adds comments in the form of notes | -c |