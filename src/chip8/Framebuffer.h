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
		static constexpr u8 MaxWidth = 128; //chip8 hires
		static constexpr u8 MaxHeight = 64;
		static constexpr size_t MaxSize = MaxWidth * MaxHeight;
		static constexpr u8 DirtyBit = 0x80;

	private:
		u8		_w, _h;
		u16		_size;

		std::array<u8, MaxSize> _data;

	public:
		Framebuffer() { Reset(); }

		Framebuffer(const Framebuffer &) = delete;
		Framebuffer& operator = (const Framebuffer &) = delete;

		u8 GetWidth() const		{ return _w; }
		u8 GetHeight() const	{ return _h; }

		void SetResolution(u8 w, u8 h)
		{
			_w = w; _h = h; _size = w * h;
			Clear(); //quirks: some game bug-2-bug compatible with octo
		}

		void Reset()
		{ SetResolution(32, 16); }

		void Invalidate() {
			for(auto & pixel : _data)
				pixel |= DirtyBit;
		}

		bool Write(u8 plane, u8 y, u8 x, u8 value)
		{
			if (value == 0)
				return false;
			//quirks, clip
			bool collision = false;

			u8 planeMask = 1 << plane;

			y %= _h;
			u16 base_offset = y * _w;

			for(u8 w = 8, srcMask = 0x80; w--; srcMask >>= 1)
			{
				size_t offset = base_offset + (x++ % _w); //fixme: quirks, clip

				if (value & srcMask) { //src pixel set
					u8 pixel = _data[offset];
					if (pixel & planeMask) //dst pixel set
					{
						pixel ^= planeMask;
						pixel |= DirtyBit;
						collision = true;
					}
					else
						pixel |= planeMask | DirtyBit;
					_data[offset] = pixel;
				}
			}
			return collision;
		}

		void Clear()
		{
			std::fill(_data.begin(), _data.end(), static_cast<u8>(DirtyBit));
		}

		u8 *GetLine(uint y)
		{ return _data.data() + y * _w; }

		void Scroll(int dx, int dy)
		{ }
	};
}

#endif

