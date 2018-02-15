#ifndef AUDIO_H
#define AUDIO_H

#include <chip8/types.h>

namespace chip8
{
	class Memory;
	class Audio
	{
		static constexpr uint	SamplingFreq	= 4000;
		static constexpr s16	Volume			= 30000;

		const Memory &	_memory;
		u16				_baseAddr;
		uint			_offset;
		u8				_currentBitOffset;
		bool			_currentBit;

	private:
		void Tick(uint freq);
		void UpdateCurrentBit();

	public:
		Audio(const Memory & memory): _memory(memory), _baseAddr(0), _offset(0), _currentBitOffset(0), _currentBit(false) { }

		void SetBaseAddr(u16 addr)
		{ _baseAddr = addr; _offset = 0; _currentBitOffset = 0; UpdateCurrentBit(); }

		void Generate(uint freq, s16 *samples, uint n);
	};
};

#endif
