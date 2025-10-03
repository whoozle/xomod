cmake_minimum_required(VERSION 3.10)
project(xomod)
include(CheckCXXCompilerFlag)

set (CMAKE_CXX_STANDARD 17)

check_cxx_compiler_flag(-Wall HAS_WALL)
if (HAS_WALL)
	add_definitions(-Wall)
endif()

check_cxx_compiler_flag(-Wimplicit-fallthrough HAS_WFALLTHROUGH)
if (HAS_WFALLTHROUGH)
	add_definitions(-Wimplicit-fallthrough)
endif()

set(XOMOD_SOURCES
	src/chip8/backend/terminal/TerminalBackend.cpp
	src/chip8/backend/sdl2/SDL2Backend.cpp

	src/chip8/Audio.cpp
	src/chip8/Chip8.cpp
	src/chip8/Config.cpp
	src/chip8/Memory.cpp
	src/main.cpp
)

add_subdirectory(src/chip8/backend/sdl2/sdl2pp)
include_directories(src src/chip8/backend/sdl2/sdl2pp ${SDL2_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/src/chip8/backend/sdl2/sdl2pp)
add_executable(xomod ${XOMOD_SOURCES})
target_link_libraries(xomod SDL2pp ${SDL2_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS xomod DESTINATION bin)
