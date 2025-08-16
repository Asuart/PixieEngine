#pragma once
#include "pch.h"
#include "TimeMeasurement.h"

namespace PixieEngine {

class Timer {
public:
	Timer(const std::string& name);
	~Timer();

private:
	std::string m_name;
	std::chrono::steady_clock::time_point m_start;
};

}