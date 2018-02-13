#include <chip8/Chip8.h>
#include <chip8/backend/terminal/TerminalBackend.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <ios>

using namespace chip8;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "usage: <rom file>" << std::endl;
		return 1;
	}

	TerminalBackend backend;
	Chip8 chip(backend);

	{
		std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		if (size < 0)
		{
			std::cerr << "could not read file " << argv[1] << std::endl;
			return 1;
		}
		file.seekg(0, std::ios::beg);

		std::vector<char> buffer(size);
		if (!file.read(buffer.data(), size))
		{
			std::cerr << "could not read file " << argv[1] << std::endl;
			return 1;
		}

		chip.Load(reinterpret_cast<const u8 *>(buffer.data()), buffer.size());
	}

	while(true)
		chip.Tick();
	return 0;
}
