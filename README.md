# midi_to_mikrotik_converter
Forum links:
<br>
https://forum.mikrotik.com/viewtopic.php?f=9&t=135207
<br>
http://www.cyberforum.ru/mikrotik/thread2033981.html
<br><br>
Video how to manually parse music to midi file and how to use that program, check time codes in video description:
<br>
https://www.youtube.com/watch?v=g6GZVlYP7X0
# Using programm

| Key | Description | Example |
| ------ | ------ | ------ |
| -f | Select input standart midi file (SMF) | -f <input_file.mid> |
| -o | Sets the octave shift relative to the original (-10 to +10) | -o -2 //Shifts all notes 2 octaves down |
| -n | Sets the note shift relative to the original | -n 3 //Shifts all notes 3 notes up |
| -b | Sets the new bpm to output file | -b 130 //Sets a new bpm to 130, ignores the original bpm in the file |
| -c | Adds comments in the form of notes | -c |
| -d | Print different level debug information while running. You can specify debug level after flag. Higher value equals more info. | -d 3 |
| -p | Sometimes it works bad, because some files have strange bytes like delay events, but really they aren't detected as pre-delay by other programs. So, if you know pre-delay is present before first note, you can enable this feature by specifying flag "-p". You need this feature, only if you want play simultaneously several instruments on each router and want to hear start of all tracks/instruments at the right time | -p |
