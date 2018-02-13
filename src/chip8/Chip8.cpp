#include <chip8/Chip8.h>
#include <chip8/String.h>
#include <chrono>
#include <string>
#include <stdexcept>

namespace chip8
{

	void Chip8::Tick()
	{
		using clock = std::chrono::high_resolution_clock;

		auto checkpoint = clock::now();
		while(true)
		{
			unsigned counter = 0;
			decltype(checkpoint) now;
			unsigned dt;
			do
			{
				for(unsigned i = 0; i < InstructionsPerStep; ++i)
					Step();

				counter += InstructionsPerStep;

				now = clock::now();
				dt = std::chrono::duration_cast<std::chrono::microseconds>(now - checkpoint).count();
			} while(dt < TimerPeriodMs);
			//printf("%u instruction per tick, %g instruction per second\n", counter, counter * 60 / (dt / 1000000.0));
			checkpoint += std::chrono::microseconds(static_cast<unsigned>(TimerPeriodMs));
		}
	}

	void Chip8::Step()
	{
		u8 h = _memory.Get(_pc++);
		u8 l = _memory.Get(_pc++);
		u8 group = h >> 4;
		u8 hl = h & 0x0f;
		u16 op = (static_cast<u16>(h) << 8) | l; //remove it

		switch(group)
		{
		case 0:
			switch(hl)
			{
			case 0:
				switch(l)
				{
					case 0xe0: //clear
						break;
					case 0xff: //hires
						break;
					default:
						throw std::runtime_error("invalid instruction " + ToHex(op));
				}
				break;

			default:
				throw std::runtime_error("invalid instruction " + ToHex(op));
			}
			break;

		case 2:
			if (_sp >= _stack.size())
				throw std::runtime_error("stack overflow");
			_stack[_sp++] = _pc;
			_pc = (static_cast<u16>(hl) << 8) | l;
			break;

		case 0x0f:
			switch(l)
			{
			case 0x01: //plane
				break;
			default:
				throw std::runtime_error("invalid instruction " + ToHex(op));
			}
			break;

		default:
			throw std::runtime_error("invalid instruction " + ToHex(op));
		}
	}

	void Chip8::Load(const u8 * data, size_t dataSize)
	{
		size_t n = std::min<size_t>(dataSize, 0x10000 - EntryPoint);
		u8 *dst = _memory.GetData() + EntryPoint;
		std::copy(data, data + n, dst);
	}

}
