#ifndef STRING_H
#define STRING_H

#include <sstream>

namespace chip8
{

	template<typename T>
	static std::string ToHex(T value, bool pad = true)
	{
		static constexpr size_t Digits = sizeof(value) * 2;
		char buffer[Digits];
		size_t idx = Digits;

		while(value && idx) {
			u8 c = value & 0x0f;
			value >>= 4;
			buffer[--idx] = '0' + (c > 9? c + 7: c);
		}

		if (pad) {
			while(idx)
				buffer[--idx] = '0';
		}
		return std::string(buffer + idx, buffer + Digits);
	}

}

#endif

