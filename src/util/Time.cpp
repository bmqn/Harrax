#include "Time.hpp"

#include <chrono>

double Time::Millis()
{
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::high_resolution_clock::now().time_since_epoch()
	).count();

	return static_cast<double>(millis) / 1000.0;
}