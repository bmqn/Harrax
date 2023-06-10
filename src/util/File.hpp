#pragma once

#include "util/Log.h"

#include <fstream>
#include <optional>
#include <vector>

std::optional<std::vector<char>> ReadFile(const std::string &path)
{
	std::ifstream fin(path, std::ios::in | std::ios::binary);
	
	if (fin)
	{
		auto eos = std::istreambuf_iterator<char>();
		auto buffer = std::vector<char>(std::istreambuf_iterator<char>(fin), eos);
		return buffer;
	}
	else
	{
		ASSERT(false, "Failed to open file '%s' !", path.c_str());
	}

	return std::nullopt;
}