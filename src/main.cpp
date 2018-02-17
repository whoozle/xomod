#include <chip8/Chip8.h>
#include <chip8/backend/terminal/TerminalBackend.h>
#include <chip8/backend/sdl2/SDL2Backend.h>
#include <chip8/File.h>
#include <chip8/Config.h>

using namespace chip8;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "usage: <rom file>" << std::endl;
		return 1;
	}

	//TerminalBackend backend;
	Config config;
	SDL2Backend backend(config);
	Chip8 chip(config, backend);

	std::string romFile = argv[1];
	{
		File rom(romFile, "rb");
		auto buffer = rom.ReadAll<std::vector<u8>>();
		chip.Load(buffer.data(), buffer.size());
	}
	auto dotPos = romFile.rfind('.');
	if (dotPos != romFile.npos)
		config.RomName = romFile.substr(0, dotPos);
	else
		config.RomName = romFile;

	std::string configFile = config.RomName + ".ini";
	if (File::Exists(configFile))
	{
		File cfg(configFile, "rt");
		auto data = cfg.ReadAll<std::vector<char>>();
		std::string text(data.begin(), data.end());
		config.Parse(text);
	}

	while(chip.Tick());
	return 0;
}
