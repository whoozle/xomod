cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_STANDARD 11)
set(OCTO_SOURCES
	src/chip8/Chip8.cpp
	src/chip8/Memory.cpp
	src/main.cpp
)

include_directories(src)
add_executable(octo ${OCTO_SOURCES})