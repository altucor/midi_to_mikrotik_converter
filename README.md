# midi_to_mikrotik_converter
[![CodeFactor](https://www.codefactor.io/repository/github/altucor/midi_to_mikrotik_converter/badge/master)](https://www.codefactor.io/repository/github/altucor/midi_to_mikrotik_converter/overview/master)\
Forum links:\
https://forum.mikrotik.com/viewtopic.php?f=9&t=135207  
http://www.cyberforum.ru/mikrotik/thread2033981.html  
Video how to manually parse music to midi file and how to use that program, check time codes in video description:\
https://www.youtube.com/watch?v=g6GZVlYP7X0
# Using program

| Short Key | Full Key | Description | Example |
| ------ | ------ | ------ |  ------ |
| -h | --help | Print this help message | -h |
| -f | --file | Select input standard midi file (SMF) | -f <input_file.mid> |
| -g | --output-file | Specify output file name. If not set than output will be saved in file with same name as input but with additional suffix | -g <output_file> |
| -o | --octave-shift | Sets the octave shift relative to the original (-10 to +10) | -o -2 //Shifts all notes 2 octaves down |
| -n | --note-shift | Sets the note shift relative to the original | -n 3 //Shifts all notes 3 notes up |
| -t | --fine-tuning | Sets frequency offset for all notes in case when you think your beeper produces beeps at wrong frequencies | -t -10.3 //Shifts all notes for 10.3Hz down |
| -b | --bpm | Sets the new bpm to output file | -b 130 //Sets a new bpm to 130, ignores the original bpm in the file |
| -c | --comments | Adds comments in the form of notes | -c |


# Building program
1) Install CMake
2) Install conan
3) cd midi_to_mikrotik_converter
4) mkdir build
5) Run only once with provider to pull BOOST library: `cmake -B build -S . -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=./conan_provider.cmake -DCMAKE_BUILD_TYPE=Debug`
6) cd build
7) cmake --build ./ --config Release

Conan CMake provider manual: https://github.com/conan-io/cmake-conan/tree/develop2

If you have some problems with getting Boost library than check available prebuilt packages and configure project to match one of available here: https://conan.io/center/boost?version=1.75.0&os=Windows&tab=configuration\
