#ifndef FILE_H
#define FILE_H

#include <chip8/types.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <stdio.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <unistd.h>

namespace chip8
{
	class File
	{
		FILE *	_f;
	public:
		File(const std::string &path) : _f(fopen(path.c_str(), "rb"))
		{
			if (!_f)
				throw std::runtime_error("could not open file " + path);
		}

		File(const File &) = delete;
		File& operator = (const File &) = delete;

		~File()
		{ fclose(_f); }

		size_t Read(void *data, size_t size)
		{ return fread(data, 1, size, _f); }
	};

	inline std::vector<u8> ReadFile(const std::string &path)
	{
		File file(path);
		std::vector<u8> data;

		size_t offset = 0;
		static constexpr size_t Step = 0x10000;

		size_t r;
		do
		{
			data.resize(offset + Step);
			r = file.Read(data.data() + offset, Step);
			offset += r;
		}
		while(r == Step);
		data.resize(offset);
		return data;
	}
}

#endif
