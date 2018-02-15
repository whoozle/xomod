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

	SDL2Backend::~SDL2Backend()
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
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					{
						bool state = event.type == SDL_KEYDOWN;
						switch(event.key.keysym.sym)
						{
						case SDLK_1: _keys[0x1] = state; break;
						case SDLK_2: _keys[0x2] = state; break;
						case SDLK_3: _keys[0x3] = state; break;
						case SDLK_4: _keys[0xc] = state; break;
						case SDLK_q: _keys[0x4] = state; break;
						case SDLK_w: _keys[0x5] = state; break;
						case SDLK_e: _keys[0x6] = state; break;
						case SDLK_r: _keys[0xd] = state; break;
						case SDLK_a: _keys[0x7] = state; break;
						case SDLK_s: _keys[0x8] = state; break;
						case SDLK_d: _keys[0x9] = state; break;
						case SDLK_f: _keys[0xe] = state; break;
						case SDLK_z: _keys[0xa] = state; break;
						case SDLK_x: _keys[0x0] = state; break;
						case SDLK_c: _keys[0xb] = state; break;
						case SDLK_v: _keys[0xf] = state; break;
						}
					}
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

		static SDL_Color palette[4] =
		{
			{ 0x28, 0x35, 0x93, 0xff },
			{ 0x4c, 0xaf, 0x50, 0xff },
			{ 0xff, 0x98, 0x00, 0xff },
			{ 0xf4, 0x43, 0x36, 0xff }
		};
		_renderer.SetDrawColor(palette[0]);
		_renderer.Clear();

		for(int y = 0; y < chipH; ++y)
		{
			auto fbLine = fb.GetLine(y);
			for(int x = 0; x < chipW; ++x)
			{
				u8 color = fbLine[x] & 0x03;
				_renderer.SetDrawColor(palette[color]);
				SDL2pp::Rect rect(offsetX + x * num, offsetY + y * num, num, num);
				_renderer.FillRect(rect);
			}
		}
		_renderer.Present();

		return running;
	}

	bool SDL2Backend::GetKeyState(u8 index)
	{
		return _keys.at(index);
	}
}
