#ifndef AUDIO_H
#define AUDIO_H

#include <chip8/types.h>
#include <random>

namespace chip8
{
	class Memory;
	class Audio
	{
		static constexpr uint	SamplingFreq	= 4000;
		static constexpr s16	VolumeMin		= 29000;
		static constexpr s16	VolumeMax		= 30000;

		const Memory &	_memory;
		u16				_baseAddr;
		uint			_offset;
		u8				_currentBitOffset;
		bool			_currentBit;

		std::default_random_engine			_randomGenerator;
		std::uniform_int_distribution<s16>	_randomDistribution;

	private:
		void Tick(uint freq);
		void UpdateCurrentBit();

	public:
		Audio(const Memory & memory): _memory(memory), _baseAddr(0), _offset(0), _currentBitOffset(0), _currentBit(false),
			_randomDistribution(VolumeMin, VolumeMax) { }

		void SetBaseAddr(u16 addr)
		{ _baseAddr = addr; _offset = 0; _currentBitOffset = 0; UpdateCurrentBit(); }

		bool GetCurrentBit() const
		{ return _currentBit; }

		void Generate(uint freq, s16 *samples, uint n);
	};
};

#endif
