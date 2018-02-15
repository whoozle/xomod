#include <chip8/Audio.h>
#include <chip8/Memory.h>

namespace chip8
{
	void Audio::UpdateCurrentBit()
	{ _currentBit = _memory.Get(_baseAddr + (_currentBitOffset >> 3)) & (0x80 >> (_currentBitOffset & 0x07)); }

	void Audio::Tick(uint freq)
	{
		_offset += SamplingFreq;
		if (_offset > freq)
		{
			_offset -= freq;
			_currentBitOffset = (_currentBitOffset + 1) & 0x7f; //128 bits
			UpdateCurrentBit();
		}
	}

	void Audio::Generate(uint freq, s16 *samples, uint n)
	{
		while(n--)
		{
			Tick(freq);
			*samples++ = _currentBit? Volume: 0;
		}
	}
}
