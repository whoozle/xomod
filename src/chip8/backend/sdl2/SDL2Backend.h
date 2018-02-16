#ifndef SDL2BACKEND_H
#define SDL2BACKEND_H

#include <chip8/Backend.h>
#include <SDL2pp/SDL.hh>
#include <SDL2pp/Window.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/AudioDevice.hh>
#include <SDL2pp/AudioSpec.hh>

namespace chip8
{
	class Config;
	class SDL2Backend : public Backend
	{
		static constexpr uint SampleFreq = 44100;

		Config &					_config;
		SDL2pp::SDL					_sdl;
		SDL2pp::Window				_window;
		SDL2pp::Renderer			_renderer;
		SDL2pp::AudioSpec			_spec;
		Audio *						_audio;

		std::array<bool, 16>		_keys;

		SDL2pp::AudioDevice			_audioDevice; //leave last member, can call back early

	private:
		void Generate(Uint8* stream, int len);

	public:
		SDL2Backend(Config & config);
		~SDL2Backend();

		bool Render(Framebuffer & fb) override;
		bool GetKeyState(u8 index) override;
		void SetAudio(Audio *audio) override;
	};
}

#endif
