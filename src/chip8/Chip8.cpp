#include <chip8/Chip8.h>
#include <chip8/String.h>
#include <chip8/Backend.h>
#include <chrono>
#include <string>
#include <stdexcept>
#include <thread>

namespace chip8
{
	namespace
	{
		u16 Pack16(u8 h, u8 l)
		{ return (static_cast<u16>(h) << 8) | l; }
	}

	void Chip8::InvalidOp(u16 op)
	{ throw std::runtime_error("invalid instruction " + ToHex(op)); }

	bool Chip8::Tick()
	{
		using clock = std::chrono::high_resolution_clock;

		auto started = clock::now();

		for(unsigned i = 0; i < _speed && _running; ++i)
			Step();

		if (!_running)
			return false;

		std::this_thread::sleep_until(started + std::chrono::microseconds((uint)TimerPeriodMs));

		if (_delay)
			--_delay;

		return _backend.Render(_framebuffer);
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
				collision |= _framebuffer.Write(plane, y, x + 8, _memory.Get(i++));
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
		u16 op = Pack16(hh, nn); //remove it

		switch(group)
		{
		case 0x0:
			switch(x)
			{
			case 0x0:
				switch(nn)
				{
					case 0xc0 ... 0xcf:
						_framebuffer.Scroll(0, nn & 0x0f); //down n pixels
						break;
					case 0xd0 ... 0xdf:
						_framebuffer.Scroll(0, -(nn & 0x0f)); //down n pixels
						break;
					case 0xe0: //clear
						_framebuffer.Clear();
						break;
					case 0xee: //ret
						if (_sp == 0)
							InvalidOp(op); //stack overflow, replace method
						_pc = _stack[--_sp];
						break;
					case 0xfb: //scroll right 4
						_framebuffer.Scroll(4, 0);
						break;
					case 0xfc: //scroll left 4
						_framebuffer.Scroll(-4, 0);
						break;
					case 0xfd: //halt
						_running = false;
						break;
					case 0xfe: //lores
						_framebuffer.SetResolution(64, 32);
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

		case 0x1: //jump NNN
			_pc = Pack16(x, nn);
			break;

		case 0x2: //call NNN
			if (_sp >= _stack.size())
				throw std::runtime_error("stack overflow");
			_stack[_sp++] = _pc;
			_pc = Pack16(x, nn);
			break;

		case 0x3: //SE VX, NN
			if (_reg[x] == nn)
				SkipNext();
			break;

		case 0x4: //SNE VX, NN
			if (_reg[x] != nn)
				SkipNext();
			break;

		case 0x5: //SXX VX, VY
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
			break;

		case 0x6: //LD VX, NN
			_reg[x] = nn;
			break;

		case 0x7:
			_reg[x] += nn;
			break;

		case 0x8: //arithmetics
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
			}
			break;

		case 0x9: //SNE VX, NN
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;
				if (z != 0)
					InvalidOp(op);

				if (_reg[x] != _reg[y])
					SkipNext();
			}
			break;

		case 0xa: //MOV I, NNN
			_i = Pack16(x, nn);
			break;

		case 0xb: //JUMP0 NNN
			_pc = Pack16(x, nn) + _reg[0];
			break;

		case 0xd: //sprite
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

		case 0xe:
			switch(nn)
			{
			case 0x9e:
				if (_backend.GetKeyState(_reg[x]))
					SkipNext();
				break;
			case 0xa1:
				if (!_backend.GetKeyState(_reg[x]))
					SkipNext();
				break;
			default:
				InvalidOp(op);
			}
			break;

		case 0xf:
			switch(nn)
			{
			case 0x00:
				if (x == 0)
					{ u8 h = _memory.Get(_pc++); u8 l = _memory.Get(_pc++); _i = Pack16(h, l); }
				else
					InvalidOp(op);
				break;

			case 0x01: //plane
				_planes = x & 0x03;
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
