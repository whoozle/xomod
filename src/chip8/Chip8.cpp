#include <chip8/Chip8.h>
#include <chip8/Config.h>
#include <chip8/String.h>
#include <chip8/Backend.h>
#include <chrono>
#include <string>
#include <stdexcept>
#include <thread>
#include <stdio.h>

//make runtime option?
#define LOG_INSTRUCTIONS 0

#if LOG_INSTRUCTIONS
#	define TRACE(...) do { printf("%04x: ", _pc - 2); printf(__VA_ARGS__); printf("\n"); } while(false)
#	define TRACEI(...) do { printf("%04x: %04x ", _pc - 2, op); printf(__VA_ARGS__); printf("\n"); } while(false)
#else
#	define TRACE(...)
#	define TRACEI(...)
#endif

#define LOG_DELAY_LOOPS 0

namespace chip8
{
	namespace
	{
		u16 Pack16(u8 h, u8 l)
		{ return (static_cast<u16>(h) << 8) | l; }
	}

	Chip8::Chip8(Config & config, Backend & backend):
		_config(config),
		_backend(backend),
		_audio(_memory),
		_randomDistribution(0, 255)
	{ Reset(); }

	void Chip8::InvalidOp(u16 op)
	{ Dump(); throw std::runtime_error("invalid instruction " + ToHex(op)); }

	bool Chip8::Tick()
	{
		using clock = std::chrono::high_resolution_clock;

		auto started = clock::now();

		if (_waitingInput)
		{
			bool anyKeyActive = false;
			for(u8 i = 0; i < 16; ++i)
			{
				if (_backend.GetKeyState(i))
				{
					_reg.at(_inputReg) = i;
					anyKeyActive = true;
					if (!_waitingInputFinished)
						TRACE("v%x = %u\n", _inputReg, i);
					_waitingInputFinished = true;
				}
			}
			if (_waitingInputFinished && !anyKeyActive)
				_waitingInput = false;
		}

		uint speed = _config.Core.Speed;
#if LOG_DELAY_LOOPS
		for(uint i = 0, lastRead = 0; i < speed; ++i)
		{
			_delayRead = false;
			Step();
			if (_delayRead)
			{
				_delayRead = false;
				printf("delay loop detected at %u with distance %d\n", _pc - 2, i - lastRead);
				lastRead = i;
			}
		}
#else
		uint n = speed;
		while (n-- && !_waitingInput && _running)
		{
			Step();
		}
#endif

		if (!_running)
			return false;

		bool running = _backend.Render(_framebuffer);

		std::this_thread::sleep_until(started + std::chrono::microseconds((uint)TimerPeriodMs));

		if (_delay)
			--_delay;

		if (_buzzer)
		{
			if (--_buzzer == 0)
				_backend.SetAudio(nullptr);
		}

		return running;
	}

	bool Chip8::Sprite(u8 plane, u8 x, u8 y, u8 h, u16 i)
	{
		bool collision = false;
		if (h == 0) //16x16 mode
		{
			for(h = 16; h--; ++y)
			{
				collision |= _framebuffer.Write(plane, y, x, _memory.Get(i++));
				collision |= _framebuffer.Write(plane, y, x + 8, _memory.Get(i++));
			}
		}
		else
		{
			while(h--)
				collision |= _framebuffer.Write(plane, y++, x, _memory.Get(i++));
		}
		return collision;
	}

	void Chip8::Step()
	{
		if (_pc < 0x200) {
			fprintf(stderr, "executing protected ROM space, halting...\n");
			Halt();
			return;
		}
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
					case 0x00:
						TRACEI("halt");
						Halt();
						break;
					case 0xc0 ... 0xcf:
						TRACEI("scroll-down %d", nn & 0x0f);
						_framebuffer.Scroll(0, nn & 0x0f); //down n pixels
						break;
					case 0xd0 ... 0xdf:
						TRACEI("scroll-up %d", nn & 0x0f);
						_framebuffer.Scroll(0, -(nn & 0x0f)); //down n pixels
						break;
					case 0xe0: //clear
						TRACEI("clear");
						_framebuffer.Clear();
						break;
					case 0xee: //ret
						TRACEI("ret");
						if (_sp == 0)
							InvalidOp(op); //stack overflow, replace method
						_pc = _stack[--_sp];
						break;
					case 0xfb: //scroll right 4
						TRACEI("scroll-right");
						_framebuffer.Scroll(4, 0);
						break;
					case 0xfc: //scroll left 4
						TRACEI("scroll-left");
						_framebuffer.Scroll(-4, 0);
						break;
					case 0xfd: //halt
						TRACEI("exit");
						Halt();
						break;
					case 0xfe: //lores
						TRACEI("lores");
						_framebuffer.SetResolution(64, 32);
						break;
					case 0xff: //hires
						TRACEI("hires");
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
			TRACEI("jump 0x%04x", Pack16(x, nn));
			_pc = Pack16(x, nn);
			break;

		case 0x2: //call NNN
			TRACEI("call 0x%04x", Pack16(x, nn));
			if (_sp >= _stack.size())
				throw std::runtime_error("stack overflow");
			_stack[_sp++] = _pc;
			_pc = Pack16(x, nn);
			break;

		case 0x3: //SE VX, NN
			TRACEI("skip-eq v%x 0x%02x", x, nn);
			if (_reg[x] == nn)
				SkipNext();
			break;

		case 0x4: //SNE VX, NN
			TRACEI("skip-ne v%x 0x%02x", x, nn);
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
					TRACEI("skip-e v%x v%x", x, y);
					if (_reg[x] == _reg[y])
						SkipNext();
					break;

				case 2: //SAVE VX-VY range
					TRACEI("save v%x-v%x", x, y);
					SaveRange(x, y);
					break;

				case 3: //LOAD VX-VY range
					TRACEI("load v%x-v%x", x, y);
					LoadRange(x, y);
					break;

				default:
					InvalidOp(op);
				}
			}
			break;

		case 0x6: //LD VX, NN
			TRACEI("v%x := 0x%02x", x, nn);
			_reg[x] = nn;
			break;

		case 0x7:
			TRACEI("v%x += 0x%02x", x, nn);
			_reg[x] += nn;
			break;

		case 0x8: //arithmetics
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;
				switch(z)
				{
				case 0x0: TRACE("v%x = v%x\n", x, y);  _reg[x]  = _reg[y]; break;
				case 0x1: TRACE("v%x |= v%x\n", x, y); _reg[x] |= _reg[y]; break;
				case 0x2: TRACE("v%x &= v%x\n", x, y); _reg[x] &= _reg[y]; break;
				case 0x3: TRACE("v%x ^= v%x\n", x, y); _reg[x] ^= _reg[y]; break;
				case 0x4: TRACE("v%x += v%x\n", x, y); { u16 r = _reg[x] + _reg[y]; WriteResult(x, r & 0xff, r > 0xff); } break;
				case 0x5: TRACE("v%x -= v%x\n", x, y); { u8  r = _reg[x] - _reg[y]; WriteResult(x, r, _reg[x] >= _reg[y]); } break;
				case 0x7: TRACE("v%x =- v%x\n", x, y); { u8  r = _reg[y] - _reg[x]; WriteResult(x, r, _reg[y] >= _reg[x]); } break;
				case 0x6:
					{
						TRACE("v%x >> v%x\n", x, y);
						if (!_config.Quirks.Shift)
						{ u8  r = _reg[y] >> 1; WriteResult(x, r, _reg[y] & 1); }
						else
						{ u8  r = _reg[x] >> 1; WriteResult(x, r, _reg[x] & 1); }
					}
					break;
				case 0xe:
					{
						TRACE("v%x << v%x\n", x, y);
						if (!_config.Quirks.Shift)
						{ u8  r = _reg[y] << 1; WriteResult(x, r, _reg[y] & 0x80); }
						else
						{ u8  r = _reg[x] << 1; WriteResult(x, r, _reg[x] & 0x80); }
					}
					break;
				}
			}
			break;

		case 0x9: //SNE VX, NN
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;

				TRACEI("skip-ne v%x v%x ; z = %u", x, y, z);

				if (z != 0)
					InvalidOp(op);

				if (_reg[x] != _reg[y])
					SkipNext();
			}
			break;

		case 0xa: //MOV I, NNN
			_i = Pack16(x, nn);
			TRACEI("i := 0x%04x", _i);
			break;

		case 0xb: //JUMP0 NNN
			TRACEI("jump0 0x%04x", Pack16(x, nn));
			_pc = Pack16(x, nn) + _reg[0];
			break;

		case 0xc:
			TRACEI("random %u", nn);
			_reg[x] = _randomDistribution(_randomGenerator) & nn;
			break;

		case 0xd: //sprite
			{
				u8 y = nn >> 4;
				u8 z = nn & 0x0f;
				TRACEI("sprite v%x v%x %u", x, y, z);
				u8 xp = _reg[x];
				u8 yp = _reg[y];
				switch(_planes)
				{
					case 1:
						_reg[VF]  = Sprite(0, xp, yp, z, _i);
						break;
					case 2:
						_reg[VF]  = Sprite(1, xp, yp, z, _i);
						break;
					case 3:
						_reg[VF]  = Sprite(0, xp, yp, z, _i);
						_reg[VF] |= Sprite(1, xp, yp, z, _i + (z == 0? 32: z));
						break;
				}
			}
			break;

		case 0xe:
			switch(nn)
			{
			case 0x9e:
				TRACEI("skip v%x key", x);
				if (_backend.GetKeyState(_reg[x]))
					SkipNext();
				break;
			case 0xa1:
				TRACEI("skip v%x -key", x);
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
				{
					u8 h = _memory.Get(_pc++);
					u8 l = _memory.Get(_pc++);
					_i = Pack16(h, l);
					TRACEI("i := long 0x%04x", _i);
				}
				else
					InvalidOp(op);
				break;

			case 0x01: //plane
				TRACEI("plane %u", x);
				_planes = x & 0x03;
				break;

			case 0x02: //audio
				TRACEI("audio");
				_audio.SetBaseAddr(_i);
				break;

			case 0x07: //vX = delay
				_reg[x] = _delay;
				TRACEI("v%x = delay ; %u\n", x, _delay);
				_delayRead = true;
				break;

			case 0x0a: //vX = key
				TRACEI("v%x := key", x);
				_waitingInput = true;
				_waitingInputFinished = false;
				_inputReg = x;
				break;

			case 0x15: //delay vX
				TRACEI("delay := v%x", x);
				_delay = _reg[x];
				break;

			case 0x18: //buzzer vX
				TRACEI("buzzer := v%x", x);
				_buzzer = _reg[x];
				_backend.SetAudio(_buzzer? &_audio: nullptr);
				break;

			case 0x1e: //i += vX
				TRACEI("i += v%x", x);
				_i += _reg[x];
				break;

			case 0x29: //hex
				TRACEI("i = hex v%x", x);
				_i = Memory::FontOffset + (_reg[x] & 0xf) * 5;
				break;

			case 0x30: //bighex
				TRACEI("i = bighex v%x", x);
				_i = Memory::BigFontOffset + (_reg[x] & 0xf) * 10;
				break;

			case 0x33: //bcd
				{
					TRACEI("bcd v%x", x);
					_memory.Set(_i + 0, (_reg[x] / 100) % 10);
					_memory.Set(_i + 1, (_reg[x] / 10) % 10);
					_memory.Set(_i + 2, _reg[x] % 10);
				}
				break;

			case 0x55: //save v0-vX
				TRACEI("save v%x", x);
				SaveRange(0, x); if (!_config.Quirks.LoadStore) _i += x + 1;
				break;

			case 0x65: //load v0-vX
				TRACEI("load v%x", x);
				LoadRange(0, x); if (!_config.Quirks.LoadStore) _i += x + 1;
				break;

			case 0x75: //export flags
				TRACEI("saveflags v%x", x);
				_config.SaveFlags(_reg.data(), x + 1);
				break;

			case 0x85: //import flags
				TRACEI("loadflags v%x", x);
				_config.LoadFlags(_reg.data(), x + 1);
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
	void Chip8::Reset()
	{
		_memory.Reset();
		_pc = EntryPoint;
		_sp = 0;
		_planes = 1;
		_delay = 0;
		_buzzer = 0;
		_running = true;
		_framebuffer.SetResolution(64, 32);
		_backend.SetAudio(nullptr);
		_waitingInput = false;
	}

	void Chip8::Dump()
	{
		fprintf(stderr, "CHIP8 halted at address pc: 0x%04x, i: 0x%04x, delay: %u, buzzer: %u\n", (uint)_pc, (uint)_i, (uint)_delay, (uint)_buzzer);
		if (_sp)
		{
			fprintf(stderr, "Stack:");
			for(uint i = 0; i < _sp; ++i)
				fprintf(stderr, " 0x%04x", _stack[i]);
			fprintf(stderr, " (top)\n");
		}
		fprintf(stderr, "Register dump:\n");
		for(uint i = 0; i < _reg.size(); ++i)
		{
			u8 value = _reg[i];
			fprintf(stderr, " v%x = 0x%02x %3u %+4d\n", i, (uint)value, (uint)value, (int)(s8)value);
		}
	}

}
