#ifndef CHIP8_H
#define CHIP8_H

#include <chip8/Memory.h>

namespace chip8
{
	class Chip8
	{
		static constexpr unsigned InstructionsPerStep = 1000;

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
			_pc = 0x200;
		}

		void Tick();
	private:
		void Step();
	};
}

#endif
