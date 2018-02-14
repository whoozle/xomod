#include <chip8/backend/terminal/TerminalBackend.h>
#include <chip8/Framebuffer.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <tuple>

namespace chip8
{
	namespace
	{
		//bits numbered as follows:
		//0 1
		//2 3
		
		const char * sub2x2[16] =
		{
			" ", "▗", "▖", "▄",
			"▝", "▐", "▞", "▟",
			"▘", "▚", "▌", "▙",
			"▀", "▜", "▛", "█",
		};

		template<uint N>
		class Sample
		{
		public:
			using Row = std::array<u8, N>;
		private:
			std::array<Row, N> _data;
		public:
			Row & operator[](uint index) {
				return _data.at(index);
			}

			std::tuple<u8, u8, u8> Quantize() const
			{
				std::array<uint, 4> stats = { 0, 0, 0, 0 }; //just four colors

				using ColorStat = std::pair<u8, uint>;
				std::array<ColorStat, 4> sorted;

				for(auto & row : _data)
					for(auto & color : row)
						++stats.at(color);

				for(u8 c = 0; c < 4; ++c)
					sorted[c] = std::make_pair(c, stats[c]);

				std::sort(sorted.begin(), sorted.end(), [](const ColorStat & a, const ColorStat &b) -> bool {
					return a.second > b.second;
				});

				u8 color1 = sorted[0].first, color2 = sorted[1].second;
				u8 mask = 0;
				int index = 0;
				for(auto & row : _data)
					for(auto & color : row)
					{
						u8 bit = color? 1: 0;
//						if (color == color1)
//							bit = 1;
//						else if (color == color2)
//							bit = 0;
//						else
//							bit = index++ & 1;
						mask <<= 1;
						mask |= bit;
					}
				return std::make_tuple(color1, color2, mask);
			}
		};
	}

	void TerminalBackend::Render(Framebuffer & fb)
	{
		struct winsize w;
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0)
			return;

		if (w.ws_col <= 0 || w.ws_row <= 0)
			return;

		int chipW = fb.GetWidth();
		int chipH = fb.GetHeight();

		//printf("terminal size %dx%d, fbsize: %dx%d\n", w.ws_col, w.ws_row, chipW, chipH);

		int num, denom, offsetX, offsetY;
		CalculateZoom(num, denom, offsetX, offsetY, w.ws_col, w.ws_row, chipW, chipH);

		//puts("\033[s");
		if (denom == 2)
		{
			std::vector<u8 *> lines(denom);
			for(int y = 0; y < chipH; y += denom, ++offsetY)
			{
				SetCursor(offsetY, offsetX);

				for(int subY = 0; subY < denom; ++subY)
					lines[subY] = fb.GetLine(y + subY);

				Sample<2> sample;
				for(int x = 0; x < chipW; x += denom)
				{
					for(int subY = 0; subY < denom; ++subY)
					{
						auto &line = lines[subY];
						for(int subX = 0; subX < denom; ++subX)
						{
							u8 value = line[x + subX] &= ~Framebuffer::DirtyBit;
							sample[subY][subX] = value;
						}
					}

					u8 color1, color2, mask;
					std::tie(color1, color2, mask) = sample.Quantize();
					Print(sub2x2[mask], 0, 7);
				}
			}
		}
		else
		{
			for(int y = 0; y < chipH; y += denom, ++offsetY)
			{
				SetCursor(offsetY, offsetX);
				auto line = fb.GetLine(y);
				for(int x = 0; x < chipW; x += denom)
				{
					auto value = line[x] &= ~Framebuffer::DirtyBit;
					Print(" ", value);
				}
			}
		}
		//puts("\033[u");
	}

	void TerminalBackend::Print(const char * what, int bgColor)
	{
		printf("\e[%dm%s", bgColor + 40, what);
	}

	void TerminalBackend::Print(const char * what, int bgColor, int fgColor)
	{
		printf("\e[%dm\e[%dm%s", fgColor + 30, bgColor + 40, what);
	}

	void TerminalBackend::SetCursor(int y, int x)
	{
		printf("\033[%d;%df", y + 1, x + 1);
	}

}
