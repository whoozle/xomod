#ifndef CONSOLEBACKEND_H
#define CONSOLEBACKEND_H

#include <chip8/Backend.h>

namespace chip8
{
	class TerminalBackend : public Backend
	{
	public:
		void Render(Framebuffer & fb) override;
	};
}

#endif
