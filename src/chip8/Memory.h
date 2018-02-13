#ifndef MEMORY_H
#define MEMORY_H

#include <chip8/types.h>
#include <array>

namespace chip8
{
	class Memory
	{
		static constexpr unsigned Size = 0x10000; //64k
		std::array<u8, Size> _data;

	public:
		static constexpr u16 FontOffset		= 0;
		static constexpr u16 FontSize		= 5 * 16;
		static constexpr u16 BigFontOffset	= FontOffset + FontSize;
		static constexpr u16 BigFontSize	= 10 * 16;

		void Reset();
	};
}


#endif
