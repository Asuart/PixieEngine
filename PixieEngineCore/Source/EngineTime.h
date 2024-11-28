#pragma once
#include "pch.h"

class Time {
public:
	static const Float fixedDeltaTime;
	static Float deltaTime;
	static std::chrono::milliseconds lastTime;

	static void Update();
};

class HightPrecisionTimer {
public:
	HightPrecisionTimer(const std::string& name);
	~HightPrecisionTimer();

protected:
	std::string m_name;
	std::chrono::steady_clock::time_point m_start;
};
