#include <chip8/backend/terminal/TerminalBackend.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

namespace chip8
{
	void TerminalBackend::Render(const Framebuffer & fb)
	{
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		printf("terminal size %dx%d\n", w.ws_col, w.ws_row);
	}
}
