#ifndef FILE_H
#define FILE_H

#include <chip8/types.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <ios>
#include <stdexcept>

namespace chip8
{
	inline std::vector<u8> ReadFile(const std::string &path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		std::streamsize size = file.tellg();
		if (size < 0)
			throw std::runtime_error("could not open file " + path);

		file.seekg(0, std::ios::beg);

		std::vector<u8> buffer(size);
		if (!file.read(static_cast<char *>(static_cast<void *>(buffer.data())), size))
			throw std::runtime_error("could not read file " + path);
		return buffer;
	}
}

#endif
