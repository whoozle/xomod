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
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0)
			return;

		if (w.ws_col <= 0 || w.ws_row <= 0)
			return;

		int chipW = fb.GetWidth();
		int chipH = fb.GetHeight();

		//printf("terminal size %dx%d, fbsize: %dx%d\n", w.ws_col, w.ws_row, chipW, chipH);

		int num, denom, offsetX, offsetY;
		CalculateZoom(num, denom, offsetX, offsetY, w.ws_col, w.ws_row, chipW, chipH);

		++offsetX;
		++offsetY;

		puts("\033[s");
		for(unsigned y = 0; y < chipH; y += denom, ++offsetY)
		{
			printf("\033[%d;%df", offsetY, offsetX);
			auto line = fb.GetLine(y);
			for(unsigned x = 0; x < chipW; x += denom)
			{
				auto value = line[x] &= ~Framebuffer::DirtyBit;
				printf("\e[%dm ", value + 40);
			}
		}
		puts("\033[u");
	}
}
