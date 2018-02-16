#include <chip8/Config.h>

namespace chip8
{
	void Config::CoreConfig::Set(const std::string& name, const std::string& value)
	{

	}

	void Config::PaletteConfig::Set(const std::string& name, const std::string& value)
	{

	}

	void Config::QuirksConfig::Set(const std::string& name, const std::string& value)
	{

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