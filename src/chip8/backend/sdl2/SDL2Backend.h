#ifndef SDL2BACKEND_H
#define SDL2BACKEND_H

#include <chip8/Backend.h>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/AudioDevice.hh>

namespace chip8
{
	class SDL2Backend : public Backend
	{
		SDL2pp::SDL					_sdl;
		SDL2pp::Window				_window;
		SDL2pp::Renderer			_renderer;
		std::array<bool, 16>		_keys;
		Audio *						_audio;
		uint						_audioFreq;

		SDL2pp::AudioDevice			_audioDevice; //leave last member, can call back early

	private:
		void Generate(Uint8* stream, int len);

	public:
		SDL2Backend();
		~SDL2Backend();

		bool Render(Framebuffer & fb) override;
		bool GetKeyState(u8 index) override;
		void SetAudio(Audio *audio) override;
	};
}

#endif
