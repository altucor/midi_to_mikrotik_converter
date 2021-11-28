cmake_minimum_required(VERSION 3.15.2)

project(midi-to-mikrotik-converter C CXX)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "-std=gnu++11 -stdlib=libc++")
IF(MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)

# b2 --build-dir=.\build toolset=msvc link=static runtime-link=static
set(BOOST_CUSTOM_DIR_BASE "Z:\\projects\\programming\\cpp\\sdks\\boost\\boost_1_77_0")
set(BOOST_ROOT ${BOOST_CUSTOM_DIR_BASE})
set(BOOST_INCLUDEDIR ${BOOST_CUSTOM_DIR_BASE}\\boost)
set(BOOST_LIBRARYDIR ${BOOST_CUSTOM_DIR_BASE}\\stage\\lib)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)

include_directories("./include")
include_directories(${BOOST_INCLUDEDIR})

file(GLOB SOURCES 
    "./include/Utils.h"
    "./include/ByteStream.h"
    "./include/MidiEventCode.h"
    "./include/MthdHeader.h"
    "./include/MtrkHeader.h"
    "./include/MidiMetaEvent.h"
    "./include/MidiEvent.h"
    "./include/EventTempo.h"
    "./include/MetaEventText.h"
    "./include/VLV.h"
    "./include/Event.h"
    "./include/Note.h"
    "./include/Mikrotik.h"
    "./include/MidiFile.h"
    "./include/MTrkChunk.h"
    "./src/Utils.cpp"
    "./src/ByteStream.cpp"
    "./src/MidiEventCode.cpp"
    "./src/MthdHeader.cpp"
    "./src/MtrkHeader.cpp"
    "./src/MidiMetaEvent.cpp"
    "./src/MidiEvent.cpp"
    "./src/EventTempo.cpp"
    "./src/MetaEventText.cpp"
    "./src/VLV.cpp"
    "./src/Event.cpp"
    "./src/Note.cpp"
    "./src/Mikrotik.cpp"
    "./src/MidiFile.cpp"
    "./src/MTrkChunk.cpp"
    "./src/midi-to-mikrotik-converter.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")


find_package(Boost 1.77.0 COMPONENTS program_options log)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()



