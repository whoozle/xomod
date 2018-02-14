#include <chip8/backend/sdl2/SDL2Backend.h>
#include <chip8/Framebuffer.h>
#include <SDL.h>

namespace chip8
{
	SDL2Backend::SDL2Backend():
		_sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS),
		_window("CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_RESIZABLE),
		_renderer(_window, -1, SDL_RENDERER_ACCELERATED)
	{ }

	bool SDL2Backend::Render(Framebuffer & fb)
	{
		bool running = true;
		{
			SDL_Event event;
			while(SDL_PollEvent(&event))
			{
				switch(event.type)
				{
				case SDL_QUIT:
					running = false;
					break;
				}
			}
		}

		int chipW = fb.GetWidth(), chipH = fb.GetHeight();
		int num, denom, offsetX, offsetY;

		CalculateZoom(num, denom, offsetX, offsetY, _window.GetWidth(), _window.GetHeight(), chipW, chipH);
		//printf("num %d, offset: %d, %d\n", num, offsetX, offsetY);
		if  (denom > 1)
			return running;

		_renderer.Clear();
		_renderer.Present();

		return running;
	}

	bool SDL2Backend::GetKeyState(u8 index)
	{
		return false;
	}
}
