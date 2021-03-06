#ifndef CONFIG_H
#define CONFIG_H

#include <chip8/IniFileParser.h>
#include <chip8/types.h>
#include <array>

namespace chip8
{
	struct Config : public IniFileParser<Config>
	{
		std::string RomName;

		struct CoreConfig
		{
			uint Speed;
			uint DelayLoop;

			CoreConfig(): Speed(1000), DelayLoop(0)
			{ }

			void Set(const std::string &name, const std::string &value);
		}
		Core;

		struct QuirksConfig
		{
			bool Shift;
			bool LoadStore;
			bool VFOrder;
			bool Clip;
			bool Jump;

			QuirksConfig(): Shift(), LoadStore(), VFOrder(), Clip(), Jump() { }

			void Set(const std::string &name, const std::string &value);
		}
		Quirks;

		struct Color
		{
			u8 R, G, B;
		};

		struct PaletteConfig
		{
			Color BG, C1, C2, BL;
			Color Buzz, Border;

			PaletteConfig():
				BG { 0x28, 0x35, 0x93 },
				C1 { 0x4c, 0xaf, 0x50 },
				C2 { 0xf4, 0x43, 0x36 },
				BL { 0xe0, 0xe0, 0xe0 },
				Buzz(BG),
				Border(BG)
			{ }

			void Set(const std::string &name, const std::string &value);
		}
		Palette;

		std::array<u8, 8> Flags;

		Config(): Flags() { }

		void SaveFlags(const u8 *data, u8 n);
		void LoadFlags(u8 *data, u8 n);

	private:
		std::string GetConfigPath();
		friend class IniFileParser<Config>;
		void OnValue(const std::string &section, const std::string &name, const std::string &value);

	};
}


#endif /* CONFIG_H */

