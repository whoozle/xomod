#include <chip8/Chip8.h>
#include <chip8/backend/terminal/TerminalBackend.h>
#include <chip8/backend/sdl2/SDL2Backend.h>
#include <chip8/File.h>

using namespace chip8;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "usage: <rom file>" << std::endl;
		return 1;
	}

	//TerminalBackend backend;
	SDL2Backend backend;
	Chip8 chip(backend);

	{
		auto buffer = ReadFile<std::vector<u8>>(argv[1]);
		chip.Load(buffer.data(), buffer.size());
	}

	while(chip.Tick());
	return 0;
}
