#ifndef CHIP8_H
#define CHIP8_H

#include <chip8/Memory.h>
#include <chip8/types.h>
#include <array>

namespace chip8
{
	class Chip8
	{
		static constexpr unsigned InstructionsPerStep	= 1000;
		static constexpr unsigned EntryPoint			= 0x200;

	private:
		Memory				_memory;
		std::array<u8, 16>	_reg;
		std::array<u16, 16>	_stack;
		u16					_pc, _i;
		u8					_sp;

	public:
		static constexpr unsigned TimerFreq = 60;
		static constexpr unsigned TimerPeriodMs = 1000000 / TimerFreq;

		Chip8() //pass backend here
		{ Reset(); }

		void Reset()
		{
			_memory.Reset();
			_pc = EntryPoint;
			_sp = 0;
		}

		void Tick();
		void Load(const u8 * data, size_t dataSize);

	private:
		void Step();
	};
}

#endif
