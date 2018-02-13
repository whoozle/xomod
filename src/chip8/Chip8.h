#ifndef CHIP8_H
#define CHIP8_H

#include <chip8/Framebuffer.h>
#include <chip8/Memory.h>
#include <chip8/types.h>
#include <array>

namespace chip8
{
	class Chip8
	{
		static constexpr unsigned InstructionsPerStep	= 1000;
		static constexpr unsigned EntryPoint			= 0x200;
		static constexpr u8 VF							= 0x0f;

	private:
		Memory				_memory;
		Framebuffer			_framebuffer;

		std::array<u8, 16>	_reg;
		std::array<u16, 16>	_stack;
		u16					_pc, _i;
		u8					_sp;
		u8					_planes;

		void WriteResult(u8 reg, u8 value, bool carry)
		{
			_reg[reg] 	= value;
			_reg[VF] 	= carry? 1: 0;
		}

		void SkipNext()
		{
			u16 next = (static_cast<u16>(_memory.Get(_pc)) << 8) | _memory.Get(_pc + 1);
			_pc += (next == 0xf000)? 4: 2; //skip long assingment
		}

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
			_planes = 1;
		}

		void Tick();
		void Load(const u8 * data, size_t dataSize);
		[[ noreturn ]] void InvalidOp(u16 op);

	private:
		void Step();
	};
}

#endif
