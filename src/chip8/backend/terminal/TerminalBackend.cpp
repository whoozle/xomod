#include <chip8/backend/terminal/TerminalBackend.h>
#include <chip8/Framebuffer.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

namespace chip8
{
	void TerminalBackend::Render(Framebuffer & fb)
	{
		struct winsize w;
		int chipW = fb.GetWidth();
		int chipH = fb.GetHeight();

		u8 * data = fb.GetLine(0);
		printf("PAINT\n");
		for(int i = 0; i < chipH; ++i) {
			for(int j = 0; j < chipW; ++j) {
				printf("%c", (*data & ~Framebuffer::DirtyBit) + '0');
				++data;
			}
			printf("\n");
		}

		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0)
			return;

		printf("terminal size %dx%d, fbsize: %dx%d\n", w.ws_col, w.ws_row, chipW, chipH);
		if (w.ws_col <= 0 || w.ws_row <= 0)
			return;

//		return;

		int num, denom, offsetX, offsetY;
		CalculateZoom(num, denom, offsetX, offsetY, w.ws_col, w.ws_row, chipW, chipH);
		//printf("%d %d %+d %+d\n", num, denom, offsetX, offsetY);
		fputs("\033[s", stdout);
		for(unsigned y = 0; y < chipH; ++y)
		{
			//printf("\033[%d;%df", y + offsetY + 1, offsetX + 1);
			auto line = fb.GetLine(y);
			for(unsigned x = 0; x < chipW; ++x)
			{
				auto value = line[x] & ~Framebuffer::DirtyBit;
				if (value)
					printf("%u, %u = %u\n", y, x, value);
				//fputc('0' + value, stdout);
			}
		}
		fputs("\033[u", stdout);
	}
}
