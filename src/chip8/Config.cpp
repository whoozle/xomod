#include <chip8/Config.h>
#include <stdlib.h>

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
			if (sscanf(value.c_str(), "#%02x%02x%02x", &r, &g, &b) != 3)
				throw std::runtime_error("invalid color value " + value);
			return Config::Color { (u8)r, (u8)g, (u8)b };
		}

	}
	void Config::CoreConfig::Set(const std::string& name, const std::string& value)
	{
		if (name == "speed")
			Speed = ParseInt(value);
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
		else if (name == "buzz")
			Buzz = ParseColor(value);
		else if (name == "border")
			Border = ParseColor(value);
		else
			throw std::runtime_error("unknown parameter palette." + name + "<");
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

}