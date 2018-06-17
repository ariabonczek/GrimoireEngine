#include "Precompiled.h"

#include <Engine/Clock.h>

#ifdef PLATFORM_PC
#include <windows.h>
#endif

namespace grim {
	
	// Ticks
	static uint64_t s_processInitTime = 0;
	static uint64_t s_clockFrequency = 0;

	// Microseconds
	static uint64_t s_frameTick = 0;
	static uint64_t s_lastFrameTime = 0;

	void InitHWClock()
	{
		GRIM_ASSERT(s_processInitTime == 0, "Tried to init the HW clock twice, this is an invalid operation");
#ifdef PLATFORM_PC
		QueryPerformanceCounter((LARGE_INTEGER*)&s_processInitTime);
		QueryPerformanceFrequency((LARGE_INTEGER*)&s_clockFrequency);
#endif
		s_frameTick = s_processInitTime;
	}

	void FrameClockTick()
	{
		const uint64_t current = GetTicks();
		s_lastFrameTime = GetDeltaTime(s_frameTick, current);
		s_frameTick = current;
	}

	uint64_t GetTicks()
	{
		uint64_t currentTime;
#ifdef PLATFORM_PC
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
#endif
		return currentTime;
	}

	uint64_t GetDeltaTime(uint64_t startTick, uint64_t endTick)
	{
		GRIM_ASSERT(s_clockFrequency != 0, "Clock Frequency is zero, are you sure you initialized the HW Clock?");

		return ((endTick - startTick) * 1000000) / s_clockFrequency;
	}

	uint64_t GetFrameTime()
	{
		return s_lastFrameTime;
	}

	uint64_t GetTimeSinceProcessStart()
	{
		const uint64_t currentTime = GetTicks();
		return GetDeltaTime(s_processInitTime, currentTime);
	}

	double TimeToSeconds(uint64_t time)
	{
		return time / 10000000.f;
	}

	double TimeToMilliseconds(uint64_t time)
	{
		return time / 1000.f;
	}
}