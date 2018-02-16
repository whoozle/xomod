#include <chip8/backend/sdl2/SDL2Backend.h>
#include <chip8/Audio.h>
#include <chip8/Config.h>
#include <chip8/Framebuffer.h>
#include <SDL2pp/AudioSpec.hh>
#include <SDL.h>
#include <algorithm>

namespace chip8
{
	SDL2Backend::SDL2Backend(Config & config):
		_config(config),
		_sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS),
		_window("CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_RESIZABLE),
		_renderer(_window, -1, SDL_RENDERER_ACCELERATED),
		_spec(SampleFreq, AUDIO_S16, 1, SampleFreq / 60),
		_audio(nullptr),
		_audioDevice
		(
			SDL2pp::Optional<std::string>(), false,
			_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE,
			[this](Uint8* stream, int len) { this->Generate(stream, len); }
		)
	{
		_audioDevice.Pause(false);
	}

	SDL2Backend::~SDL2Backend()
	{
		_audioDevice.Pause(true);
		SDL2Backend::SetAudio(nullptr);
	}

	bool SDL2Backend::Render(Framebuffer & fb)
	{
		int chipW = fb.GetWidth(), chipH = fb.GetHeight();
		int num, denom, offsetX, offsetY;
		CalculateZoom(num, denom, offsetX, offsetY, _window.GetWidth(), _window.GetHeight(), chipW, chipH);

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

						case SDLK_PLUS:
						case SDLK_EQUALS:
							if (state)
							{
								int w = (num + 1)* fb.GetWidth(), h = (num + 1) * fb.GetHeight();
								fprintf(stderr, "setting window size to %dx%d\n", w, h);
								_window.SetSize(w, h);
								CalculateZoom(num, denom, offsetX, offsetY, _window.GetWidth(), _window.GetHeight(), chipW, chipH);
							}
							break;

						case SDLK_MINUS:
							if (state && num > 1)
							{
								int w = (num - 1)* fb.GetWidth(), h = (num - 1) * fb.GetHeight();
								fprintf(stderr, "setting window size to %dx%d\n", w, h);
								_window.SetSize(w, h);
								CalculateZoom(num, denom, offsetX, offsetY, _window.GetWidth(), _window.GetHeight(), chipW, chipH);
							}
							break;
						case SDLK_RETURN:
							if (state && (event.key.keysym.mod & KMOD_LALT))
							{
								auto flags = _window.GetFlags();
								bool fullscreen = (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP;
								_window.SetFullscreen(fullscreen? flags & ~SDL_WINDOW_FULLSCREEN_DESKTOP: flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
							}
							break;
						}
					}
					break;
				}
			}
		}

		//printf("num %d, offset: %d, %d\n", num, offsetX, offsetY);
		if  (denom > 1)
			return running;

		auto & P = _config.Palette;

		SDL_Color palette[4] =
		{
			{ P.BG.R, P.BG.G, P.BG.B, 0xff },
			{ P.C1.R, P.C1.G, P.C1.B, 0xff },
			{ P.C2.R, P.C2.G, P.C2.B, 0xff },
			{ P.BL.R, P.BL.G, P.BL.B, 0xff },
		};

		SDL_Color border[2] =
		{
			{ P.Border.R, P.Border.G, P.Border.B, 0xff },
			{ P.Buzz.R, P.Buzz.G, P.Buzz.B, 0xff },
		};

		bool buzz = _audio? _audio->GetCurrentBit(): false;
		_renderer.SetDrawColor(border[buzz? 1: 0]);
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

	void SDL2Backend::SetAudio(Audio *audio)
	{
		auto lock = _audioDevice.Lock();
		_audio = audio;
	}

	void SDL2Backend::Generate(Uint8* stream, int len)
	{
		if (!_audio)
		{
			std::fill(stream, stream + len, 0);
			return;
		}
		_audio->Generate(_spec.freq, reinterpret_cast<s16 *>(stream), len / 2);
	}
}
