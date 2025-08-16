#pragma once
#include "pch.h"
#include "TimeMeasurement.h"

namespace PixieEngine {

class GlobalTimer {
public:
	static void StartTimer(const std::string& name);
	static void StopTimer(const std::string& name);
	static TimeMeasurement& GetTimer(const std::string& name);

private:
	inline static std::vector<TimeMeasurement> s_timers = {};
};

}