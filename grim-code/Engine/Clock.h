#pragma once

typedef unsigned long long uint64_t;

namespace grim {

	void InitHWClock();
	void FrameClockTick();

	// clock
	uint64_t GetTicks();

	// Measured in microseconds
	uint64_t GetDeltaTime(uint64_t startTick, uint64_t endTick);
	uint64_t GetFrameTime();

	uint64_t GetTimeSinceProcessStart();

	double TimeToSeconds(uint64_t time);
	double TimeToMilliseconds(uint64_t time);
}