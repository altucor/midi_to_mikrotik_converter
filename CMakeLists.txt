cmake_minimum_required(VERSION 3.24)

project(midi-to-mikrotik-converter C CXX)

set(CMAKE_CXX_STANDARD 14)

IF(MSVC)
  set(PROJECT_NAME "${PROJECT_NAME}-win")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MT")
ELSEIF(APPLE)
  set(PROJECT_NAME "${PROJECT_NAME}-osx")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ELSEIF(UNIX)
  set(PROJECT_NAME "${PROJECT_NAME}-linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF()

set(REQUIRED_BOOST_VERSION 1.86.0)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  message("Release build.")
ELSE()
  message(" ! ! ! Unknown build type.")
ENDIF()

message("CMAKEFLAGS DUMP: ${CMAKE_CXX_FLAGS}")

add_subdirectory(libmidi)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_NO_SYSTEM_PATHS TRUE)
find_package(Boost COMPONENTS program_options log REQUIRED)

include_directories("./libmidi/src")
include_directories(${Boost_INCLUDE_DIRS})
include_directories("./include")

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES "src/**.cpp" "src/**.hpp")

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

IF(MSVC)
  set_property(
    TARGET
    ${PROJECT_NAME}
    PROPERTY
    MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:>:>"
  )
ENDIF(MSVC)

target_link_libraries(${PROJECT_NAME} PRIVATE boost::boost midi)
