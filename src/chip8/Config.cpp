#include <chip8/Config.h>
#include <chip8/File.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

namespace chip8
{
	namespace
	{

		int ParseInt(const std::string &value)
		{ return atoi(value.c_str()); }

		bool ParseBoolean(const std::string &value)
		{
			if (value == "on" || value == "1" || value == "true")
				return true;
			if (value == "off" || value == "0" || value == "false")
				return true;
			throw std::runtime_error("invalid boolean value " + value);
		}

		Config::Color ParseColor(const std::string &value)
		{
			if (value.empty() || value[0] != '#')
				throw std::runtime_error("invalid prefix for color value " + value);
			uint r, g, b;
			switch(value.size())
			{
			case 7:
				if (sscanf(value.c_str(), "#%02x%02x%02x", &r, &g, &b) != 3)
					throw std::runtime_error("invalid 7-color value " + value);
				return Config::Color { (u8)r, (u8)g, (u8)b };
			case 4:
				if (sscanf(value.c_str(), "#%1x%1x%1x", &r, &g, &b) != 3)
					throw std::runtime_error("invalid 3-color value " + value);
				r = (r << 4) | r;
				g = (g << 4) | g;
				b = (b << 4) | b;
				return Config::Color { (u8)r, (u8)g, (u8)b };
			default:
				throw std::runtime_error("invalid color value " + value);
			}
		}

	}
	void Config::CoreConfig::Set(const std::string& name, const std::string& value)
	{
		if (name == "speed")
			Speed = ParseInt(value);
		else if (name == "delayloop")
			DelayLoop = ParseInt(value);
		else
			throw std::runtime_error("unknown parameter core." + name);
	}

	void Config::PaletteConfig::Set(const std::string& name, const std::string& value)
	{
		if (name == "bg")
			BG = ParseColor(value);
		else if (name == "color1")
			C1 = ParseColor(value);
		else if (name == "color2")
			C2 = ParseColor(value);
		else if (name == "blend")
			BL = ParseColor(value);
		else if (name == "buzzer")
			Buzz = ParseColor(value);
		else if (name == "border")
			Border = ParseColor(value);
		else
			throw std::runtime_error("unknown parameter palette." + name);
	}

	void Config::QuirksConfig::Set(const std::string& name, const std::string& value)
	{
		if (name == "shift")
			Shift = ParseBoolean(value);
		else if (name == "loadstore")
			LoadStore = ParseBoolean(value);
		else if (name == "vforder")
			VFOrder = ParseBoolean(value);
		else if (name == "clip")
			Clip = ParseBoolean(value);
		else if (name == "jump")
			Jump = ParseBoolean(value);
		else
			throw std::runtime_error("unknown parameter quirks." + name);
	}


	void Config::OnValue(const std::string &section, const std::string &name, const std::string &value)
	{
		if (section == "core")
			Core.Set(name, value);
		else if (section == "quirks")
			Quirks.Set(name, value);
		else if (section == "palette")
			Palette.Set(name, value);
		else
			throw std::runtime_error("unknown section " + section);
	}

	std::string Config::GetConfigPath()
	{
		const char *home = getenv("HOME");
		if (home == NULL)
			home = ".";
		std::string local = std::string(home) + "/.local";
		mkdir(local.c_str(), 0770);
		std::string share = local + "/share";
		mkdir(share.c_str(), 0770);
		std::string xomod = share + "/xomod";
		mkdir(xomod.c_str(), 0770);
		return xomod;
	}

	void Config::SaveFlags(const u8 * data, u8 n)
	{
		auto configDir = GetConfigPath();
		File file(configDir + "/" + RomName + ".flags", "wb");
		file.Write(data, n);
	}

	void Config::LoadFlags(u8 *data, u8 n)
	{
		auto configDir = GetConfigPath();
		auto flagsFile = configDir + "/" + RomName + ".flags";

		memset(data, 0, n);
		if (File::Exists(flagsFile))
		{
			File file(flagsFile, "rb");
			file.Read(data, n);
		}
	}

}