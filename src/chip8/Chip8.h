#ifndef CHIP8_H
#define CHIP8_H

#include <chip8/Audio.h>
#include <chip8/Framebuffer.h>
#include <chip8/Memory.h>
#include <chip8/types.h>
#include <array>
#include <random>

namespace chip8
{
	class Backend;
	struct Config;

	class Chip8
	{
		static constexpr uint EntryPoint			= 0x200;
		static constexpr u8 VF						= 0x0f;

	private:
		Config &			_config;
		Backend &			_backend;
		Memory				_memory;
		Framebuffer			_framebuffer;
		Audio				_audio;

		std::array<u8, 16>	_reg;
		std::array<u16, 16>	_stack;
		u16					_pc, _i;
		u8					_sp;
		u8					_planes;
		u8					_delay;
		u8					_buzzer;
		bool				_running;
		bool				_waitingInput;
		bool				_waitingInputFinished;
		u8					_inputReg;
		bool				_delayRead;

		std::default_random_engine _randomGenerator;
		std::uniform_int_distribution<u8> _randomDistribution;

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

		void SaveRange(u8 x, u8 y)
		{
			if (x < y)
				for(u8 i = 0; i <= y - x; ++i) _memory.Set(_i + i, _reg[x + i]);
			else
				for(u8 i = 0; i <= x - y; ++i) _memory.Set(_i + i, _reg[x - i]);
		}

		void LoadRange(u8 x, u8 y)
		{
			if (x < y)
				for(u8 i = 0; i <= y - x; ++i) _reg[x + i] = _memory.Get(_i + i);
			else
				for(u8 i = 0; i <= x - y; ++i) _reg[x - i] = _memory.Get(_i + i);
		}

		void DumpRange(u8 x, u8 y)
		{
			printf("%04x i 0x%04x ", _pc - 2, _i);
			if (x < y)
				for(u8 i = 0; i <= y - x; ++i) printf("v%x 0x%02x ", x + i, _reg[x + i]);
			else
				for(u8 i = 0; i <= x - y; ++i) printf("v%x 0x%02x ", x - i, _reg[x - i]);
			printf("\n");
		}

		bool Sprite(u8 plane, u8 x, u8 y, u8 h, u16 i);

	public:
		static constexpr uint TimerFreq = 60;
		static constexpr uint TimerPeriodMs = 1000000 / TimerFreq;

		Chip8(Config & config, Backend & backend);

		void Reset();

		bool Tick();
		void Load(const u8 * data, size_t dataSize);
		void Halt()
		{ _running = false; Dump(); }

		[[ noreturn ]] void InvalidOp(u16 op);
		void Dump();

	private:
		void Step();
	};
}

#endif
