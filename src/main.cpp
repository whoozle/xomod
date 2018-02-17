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

	std::string prefix;
	if (dotPos != romFile.npos)
		prefix = romFile.substr(0, dotPos);
	else
		prefix = romFile;
	{
		auto slash = prefix.rfind('/');
		if (slash == prefix.npos)
			slash = 0;
		else
			++slash;
		config.RomName = prefix.substr(slash);
	}

	std::string configFile = prefix + ".ini";
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
