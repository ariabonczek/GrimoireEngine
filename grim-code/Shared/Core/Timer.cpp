#include "Timer.h"

static Clock::time_point s_processStart;
static Clock::time_point s_lastFrame;
static Clock::duration s_frameTime;

void Clock::Initialize(void)
{
	s_processStart = std::chrono::system_clock::now();
}

void Clock::FrameTick(void)
{
	const time_point current = std::chrono::system_clock::now();
	
	s_frameTime = current - s_lastFrame;
	s_lastFrame = current;
}

float Clock::GetFrameTime(void)
{ 
	return s_frameTime.count(); 
}

float Clock::GetTimeSinceProcessStart(void)
{ 
	return duration(GetNow() - s_processStart).count();
}

Clock::time_point Clock::GetNow()
{
	return std::chrono::system_clock::now();
}

float Clock::GetElapsedTime(const time_point a, const time_point b)
{
	return duration(a - b).count();
}