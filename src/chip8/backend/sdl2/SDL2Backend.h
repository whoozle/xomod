#ifndef SDL2BACKEND_H
#define SDL2BACKEND_H

#include <chip8/Backend.h>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>

namespace chip8
{
	class SDL2Backend : public Backend
	{
		SDL2pp::SDL					_sdl;
		SDL2pp::Window				_window;
		SDL2pp::Renderer			_renderer;
		std::array<bool, 16>		_keys;

	public:
		SDL2Backend();
		~SDL2Backend();

		virtual bool Render(Framebuffer & fb) override;
		virtual bool GetKeyState(u8 index) override;
	};
}

#endif
