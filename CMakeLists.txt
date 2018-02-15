cmake_minimum_required(VERSION 2.6)
include(CheckCXXCompilerFlag)

set (CMAKE_CXX_STANDARD 11)

check_cxx_compiler_flag(-Wall HAS_WALL)
if (HAS_WALL)
	add_definitions(-Wall)
endif()

check_cxx_compiler_flag(-Wimplicit-fallthrough HAS_WFALLTHROUGH)
if (HAS_WFALLTHROUGH)
	add_definitions(-Wimplicit-fallthrough)
endif()

set(OCTO_SOURCES
	src/chip8/backend/terminal/TerminalBackend.cpp
	src/chip8/backend/sdl2/SDL2Backend.cpp

	src/chip8/Audio.cpp
	src/chip8/Chip8.cpp
	src/chip8/Memory.cpp
	src/main.cpp
)

add_subdirectory(src/chip8/backend/sdl2/sdl2pp)
include_directories(src src/chip8/backend/sdl2/sdl2pp ${SDL2_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/src/chip8/backend/sdl2/sdl2pp)
add_executable(octo ${OCTO_SOURCES})
target_link_libraries(octo SDL2pp ${SDL2_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
