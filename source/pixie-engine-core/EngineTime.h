#pragma once
#include "pch.h"

class Time {
public:
	static const float fixedDeltaTime;
	static float deltaTime;
	static std::chrono::milliseconds lastTime;

	static void Update();
};

struct HighPrecisionTimeMeasurement {
	std::string m_name;
	std::chrono::steady_clock::time_point m_start;
	std::chrono::steady_clock::time_point m_end;
	std::chrono::nanoseconds m_lastDelta;
};

class HighPrecisionTimer {
public:
	HighPrecisionTimer(const std::string& name);
	~HighPrecisionTimer();

protected:
	std::string m_name;
	std::chrono::steady_clock::time_point m_start;

public:
	static std::vector<HighPrecisionTimeMeasurement> s_timers;

	static void StartTimer(const std::string& name);
	static void StopTimer(const std::string& name);
	static HighPrecisionTimeMeasurement& GetTimer(const std::string& name);
};
