#ifndef MEMORY_H
#define MEMORY_H

#include <chip8/types.h>
#include <array>

namespace chip8
{
	class Memory
	{
	public:
		static constexpr uint Size = 0x10000; //64k
		static constexpr u16 FontOffset		= 0;
		static constexpr u16 FontSize		= 5 * 16;
		static constexpr u16 BigFontOffset	= FontOffset + FontSize;
		static constexpr u16 BigFontSize	= 10 * 16;

	private:
		std::array<u8, Size> _data;

	public:

		void Reset();

		u8 Get(u16 index)
		{ return _data[index]; }

		u8 Get(u16 index) const
		{ return _data[index]; }

		void Set(u16 index, u8 value)
		{ _data[index] = value; }

		u8 * GetData()
		{ return _data.data(); }

		const u8 * GetData() const
		{ return _data.data(); }
	};
}


#endif
