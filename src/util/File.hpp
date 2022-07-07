#pragma once

#include "util/Log.h"

#include <cstdint>
#include <fstream>
#include <vector>

std::vector<uint8_t> ReadFile(const std::string &path)
{
	std::vector<uint8_t> buffer;
	std::ifstream fin(path, std::ios::in | std::ios::binary);
	
	if (fin)
	{
		fin.seekg(0, std::ios::end);
		size_t size = fin.tellg();
		if (size != -1)
		{
			buffer.resize(size);
			fin.seekg(0, std::ios::beg);
			fin.read(reinterpret_cast<char*>(buffer.data()), size);
		}
		else
		{
			ASSERT(false, "Failed to read from file '%s' !", path.c_str());
		}
	}
	else
	{
		ASSERT(false, "Failed to open file '%s' !", path.c_str());
	}

	return buffer;
}