#pragma once
#include <chrono>
class Timer
{
public:
	Timer() noexcept;
	
	//returns elapsed time and resets timer
	float Reset() noexcept;

	//reads elapsed time, doesn't reset timer
	float GetElapsed() const noexcept;
private:
	std::chrono::steady_clock::time_point last;
	
};
