#ifndef PIXIE_ENGINE_TIME
#define PIXIE_ENGINE_TIME

#include "pch.h"

struct TimeRecord {
	std::string name;
	std::chrono::microseconds start;
	std::chrono::microseconds end;
};

class Time {
public:
	static std::chrono::microseconds lastTime;
	static float deltaTime;
	static std::vector<TimeRecord> records;

	static void Update();
	static void PrintDeltaTime();

	static TimeRecord MeasureStart(const std::string& name);
	static TimeRecord MeasureEnd(const std::string& name);

	static std::chrono::microseconds GetTime();
};

#endif // PIXIE_ENGINE_TIME