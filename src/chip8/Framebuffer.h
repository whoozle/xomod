#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <chip8/types.h>
#include <array>
#include <algorithm>

namespace chip8
{
	class Framebuffer
	{
	public:
		static const u8 MaxWidth = 128; //chip10
		static const u8 MaxHeight = 64;
		static const size_t MaxSize = MaxWidth * MaxHeight;

	private:
		u8 _w, _h;

		std::array<u8, MaxSize> _data;

	public:
		Framebuffer() { Reset(); }
		void SetResolution(u8 w, u8 h)
		{ _w = w; _h = h; }

		void Reset()
		{ SetResolution(32, 16); }

		void Invalidate() { }

		void Clear()
		{ std::fill(_data.begin(), _data.end(), 0); }
	};
}

#endif

