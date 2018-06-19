#pragma once

#include <chrono>

namespace Clock
{
	typedef std::chrono::system_clock::time_point time_point;
	typedef std::chrono::duration<float> duration;

	void Initialize();
	void FrameTick();

	float GetFrameTime(void);
	float GetTimeSinceProcessStart(void);

	time_point GetNow();
	float GetElapsedTime(const time_point a, const time_point b);
}