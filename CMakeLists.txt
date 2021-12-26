cmake_minimum_required(VERSION 3.15.2)

project(midi-to-mikrotik-converter C CXX)

set(REQUIRED_BOOST_VERSION 1.75.0)
IF(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MT")
ELSE()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
ENDIF(MSVC)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  message("Release build.")
ELSE()
  message(" ! ! ! Unknown build type.")
ENDIF()

message("CMAKEFLAGS DUMP: ${CMAKE_CXX_FLAGS}")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES boost/${REQUIRED_BOOST_VERSION}
                      GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost COMPONENTS program_options log REQUIRED)

include_directories("./include")

file(GLOB SOURCES 
    "./include/Utils.hpp"
    "./include/ByteStream.hpp"
    "./include/MidiEventCode.hpp"
    "./include/MthdHeader.hpp"
    "./include/MidiTrack.hpp"
    "./include/MidiMetaEvent.hpp"
    "./include/MidiEvent.hpp"
    "./include/EventTempo.hpp"
    "./include/MetaEventText.hpp"
    "./include/VLV.hpp"
    "./include/Event.hpp"
    "./include/Note.hpp"
    "./include/Mikrotik.hpp"
    "./include/MidiFile.hpp"
    "./src/Utils.cpp"
    "./src/ByteStream.cpp"
    "./src/MidiEventCode.cpp"
    "./src/MthdHeader.cpp"
    "./src/MidiTrack.cpp"
    "./src/MidiMetaEvent.cpp"
    "./src/MidiEvent.cpp"
    "./src/EventTempo.cpp"
    "./src/MetaEventText.cpp"
    "./src/VLV.cpp"
    "./src/Event.cpp"
    "./src/Note.cpp"
    "./src/Mikrotik.cpp"
    "./src/MidiFile.cpp"
    "./src/midi-to-mikrotik-converter.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
IF(MSVC)
    set_property(
        TARGET 
        ${PROJECT_NAME} 
        PROPERTY 
        MSVC_RUNTIME_LIBRARY 
        "MultiThreaded$<$<CONFIG:>:>"
    )
ENDIF(MSVC)

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Boost::Boost)

