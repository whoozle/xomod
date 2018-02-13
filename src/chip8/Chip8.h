#ifndef CHIP8_H
#define CHIP8_H

#include <chip8/Memory.h>

namespace chip8
{
	class Chip8
	{
		static constexpr unsigned InstructionsPerStep	= 1000;
		static constexpr unsigned EntryPoint			= 0x200;

	private:
		Memory	_memory;
		u8		_reg[16];
		u16		_pc, _i;

	public:
		static constexpr unsigned TimerFreq = 60;
		static constexpr unsigned TimerPeriodMs = 1000000 / TimerFreq;

		Chip8() //pass backend here
		{ Reset(); }

		void Reset()
		{
			_memory.Reset();
			_pc = EntryPoint;
		}

		void Tick();
		void Load(const u8 * data, size_t dataSize);

	private:
		void Step();
	};
}

#endif
