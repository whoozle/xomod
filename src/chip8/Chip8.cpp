#include <chip8/Chip8.h>
#include <chip8/String.h>
#include <chip8/Backend.h>
#include <chrono>
#include <string>
#include <stdexcept>

namespace chip8
{
	void Chip8::InvalidOp(u16 op)
	{ throw std::runtime_error("invalid instruction " + ToHex(op)); }

	void Chip8::Tick()
	{
		using clock = std::chrono::high_resolution_clock;

		auto checkpoint = clock::now();

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
		//checkpoint += std::chrono::microseconds(static_cast<unsigned>(TimerPeriodMs));
		if (_delay)
			--_delay;

		_backend.Render(_framebuffer);
	}

	void Chip8::Sprite(u8 plane, u8 x, u8 y, u8 h)
	{
		bool collision = false;
		u16 i = _i;
		if (h == 0) //16x16 mode
		{
			i += 32 * plane;
			for(h = 16; h--; ++y)
			{
				collision |= _framebuffer.Write(plane, y, x, _memory.Get(i++));
				collision |= _framebuffer.Write(plane, y, x + 1, _memory.Get(i++));
			}
		}
		else
		{
			i += h * plane;
			while(h--)
				collision |= _framebuffer.Write(plane, y++, x, _memory.Get(i++));
		}
		_reg[VF] = collision? 1: 0;
	}

	void Chip8::Step()
	{
		u8 hh = _memory.Get(_pc++);
		u8 nn = _memory.Get(_pc++);
		u8 group = hh >> 4;
		u8 x = hh & 0x0f;
		u16 op = (static_cast<u16>(hh) << 8) | nn; //remove it

		switch(group)
		{
		case 0x00:
			switch(x)
			{
			case 0x00:
				switch(nn)
				{
					case 0xe0: //clear
						_framebuffer.Clear();
						break;
					case 0xee: //ret
						if (_sp == 0)
							InvalidOp(op); //stack overflow, replace method
						_pc = _stack[--_sp];
						break;
					case 0xff: //hires
						_framebuffer.SetResolution(128, 64);
						break;
					default:
						InvalidOp(op);
				}
				break;

			default:
				InvalidOp(op);
			}
			break;

		case 0x01: //jump NNN
			_pc = (static_cast<u16>(x) << 8) | nn;
			break;

		case 0x02: //call NNN
			if (_sp >= _stack.size())
				throw std::runtime_error("stack overflow");
			_stack[_sp++] = _pc;
			_pc = (static_cast<u16>(x) << 8) | nn;
			break;

		case 0x03: //SE VX, NN
			if (_reg[x] == nn)
				SkipNext();
			break;

		case 0x04: //SNE VX, NN
			if (_reg[x] != nn)
				SkipNext();
			break;

		case 0x05: //SXX VX, VY
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;
				switch(z)
				{
				case 0: //SE VX, VY
					if (_reg[x] == _reg[y])
						SkipNext();
					break;

				case 2: //SAVE VX-VY range
					SaveRange(x, y);
					break;

				case 3: //LOAD VX-VY range
					LoadRange(x, y);
					break;

				default:
					InvalidOp(op);
				}
			}

		case 0x06: //LD VX, NN
			_reg[x] = nn;
			break;

		case 0x07:
			_reg[x] += nn;
			break;

		case 0x08: //arithmetics
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;
				switch(z)
				{
				case 0x00: _reg[x]  = _reg[y]; break;
				case 0x01: _reg[x] |= _reg[y]; break;
				case 0x02: _reg[x] &= _reg[y]; break;
				case 0x03: _reg[x] ^= _reg[y]; break;
				case 0x04: { u16 r = _reg[x] + _reg[y]; WriteResult(x, r & 0xff, r > 0xff); } break;
				case 0x05: { u8 r = _reg[x] - _reg[y]; WriteResult(x, r, _reg[x] >= _reg[y]); } break;
				case 0x07: { u8 r = _reg[y] - _reg[x]; WriteResult(x, r, _reg[y] >= _reg[x]); } break;
				case 0x06: { u8 r = _reg[x] >> 1; WriteResult(x, r, _reg[x] & 1); } break;
				case 0x0e: { u8 r = _reg[x] << 1; WriteResult(x, r, _reg[x] & 0x80); } break;
				}
			break;
			}

		case 0x09: //SNE VX, NN
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;
				if (z != 0)
					InvalidOp(op);

				if (_reg[x] != _reg[y])
					SkipNext();
			}
			break;

		case 0x0a: //MOV I, NNN
			_i = (static_cast<u16>(x) << 8) | nn;
			break;

		case 0x0b: //JUMP0 NNN
			_pc = ((static_cast<u16>(x) << 8) | nn) + _reg[0];
			break;

		case 0x0d: //sprite
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;
				u8 xp = _reg[x];
				u8 yp = _reg[y];
				switch(_planes)
				{
					case 1:
						Sprite(0, xp, yp, z);
						break;
					case 2:
						Sprite(1, xp, yp, z);
						break;
					case 3:
						Sprite(0, xp, yp, z);
						Sprite(1, xp, yp, z);
						break;
				}
			}
			break;

		case 0x0f:
			switch(nn)
			{
			case 0x00:
				if (x == 0)
					{ u8 h = _memory.Get(_pc++); u8 l = _memory.Get(_pc++); _i = (static_cast<u16>(h) << 8) | l; }
				else
					InvalidOp(op);
				break;

			case 0x01: //plane
				_planes = x;
				_framebuffer.Invalidate();
				break;

			case 0x02: //audio
				break;

			case 0x07: //vX = delay
				_reg[x] = _delay;
				break;

			case 0x15: //delay vX
				_delay = _reg[x];
				break;

			case 0x18: //buzzer vX
				break;

			case 0x1e: //i += vX
				_i += _reg[x];
				break;

			case 0x55: //save v0-vX
				SaveRange(0, x); _i += x; //quirks
				break;

			case 0x65: //load v0-vX
				LoadRange(0, x); _i += x;
				break;

			default:
				InvalidOp(op);
			}
			break;

		default:
			InvalidOp(op);
		}
	}

	void Chip8::Load(const u8 * data, size_t dataSize)
	{
		size_t n = std::min<size_t>(dataSize, 0x10000 - EntryPoint);
		u8 *dst = _memory.GetData() + EntryPoint;
		std::copy(data, data + n, dst);
	}

}
