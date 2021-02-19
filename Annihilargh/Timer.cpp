#include "Timer.h"

#include <chrono>

Timer::Timer() noexcept
{
	last=std::chrono::steady_clock::now();
}

float Timer::Reset() noexcept
{
	const auto old = last;
	last = std::chrono::steady_clock::now();
	const std::chrono::duration<float> elapsed = last-old;
	return elapsed.count();
}

float Timer::GetElapsed() const noexcept
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now()-last).count();
}
