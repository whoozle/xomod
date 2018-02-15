#ifndef AUDIO_H
#define AUDIO_H

#include <chip8/types.h>

namespace chip8
{
	class Memory;
	class Audio
	{
		const Memory &	_memory;
		u16				_baseAddr;

	public:
		Audio(const Memory & memory): _memory(memory), _baseAddr(0) { }

		void SetBaseAddr(u16 addr)
		{ _baseAddr = addr; }
	};
};

#endif
