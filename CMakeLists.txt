cmake_minimum_required (VERSION 3.0)
project (libworkflow)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_STANDARD 11)
ADD_DEFINITIONS(
    -std=gnu++11
    )

add_definitions(-Wno-deprecated-register)
add_definitions(-DBOOST_ALL_DYN_LINK)
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE( Boost 1.5.5 COMPONENTS system log filesystem thread date_time timer serialization regex iostreams chrono log_setup REQUIRED ) 
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} .)

file(GLOB_RECURSE files core/*.cpp actions/*.cpp contexts/*.cpp tools/*.cpp service/*.cpp)

add_library(libworkflow SHARED ${files})
TARGET_LINK_LIBRARIES(libworkflow ${Boost_LIBRARIES} pthread)

add_subdirectory(tests)

