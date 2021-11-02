cmake_minimum_required(VERSION 3.10)

# Main Project
project(FlameUI VERSION 1.0)

# C++17 or higher version of C++ Required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# Handle Generation for single config generators like MakeFiles, when no Config is specified explicitly in the command line
if(CMAKE_BUILD_TYPE STREQUAL "")
set(CMAKE_BUILD_TYPE "Debug")
message("-- CMAKE_BUILD_TYPE not specified, setting Debug as default configuration.")
endif()

set(FL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")

# Include util functions and variables with paths to library dependencies
include(CMakeUtils/Dependencies.cmake)
include(CMakeUtils/CMakeUtils.cmake)

# Add the actual library directory and the Sandbox project
add_subdirectory(FlameUI)
add_subdirectory(Sandbox)