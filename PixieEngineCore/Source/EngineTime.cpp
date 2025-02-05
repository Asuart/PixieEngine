#include "pch.h"
#include "EngineTime.h"

const Float Time::fixedDeltaTime = 0.002f;
Float Time::deltaTime = 1.0f / 60.0f;
std::chrono::milliseconds Time::lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
std::vector<HighPrecisionTimeMeasurement> HighPrecisionTimer::s_timers;

void Time::Update() {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	deltaTime = (newTime - lastTime).count() / 1000.0f;
	lastTime = newTime;
}

HighPrecisionTimer::HighPrecisionTimer(const std::string& name) : m_name(name) {
	m_start = std::chrono::high_resolution_clock::now();
}

HighPrecisionTimer::~HighPrecisionTimer() {
	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::cout << m_name << ": " << (end - m_start) << "\n";
}

void HighPrecisionTimer::StartTimer(const std::string& name) {
	HighPrecisionTimeMeasurement& timer = GetTimer(name);
	timer.m_start = std::chrono::high_resolution_clock::now();
}

void HighPrecisionTimer::StopTimer(const std::string& name) {
	HighPrecisionTimeMeasurement& timer = GetTimer(name);
	timer.m_end = std::chrono::high_resolution_clock::now();
	timer.m_lastDelta = timer.m_end - timer.m_start;
}

HighPrecisionTimeMeasurement& HighPrecisionTimer::GetTimer(const std::string& name) {
	for (int32_t i = 0; i < s_timers.size(); i++) {
		if (s_timers[i].m_name == name) {
			return s_timers[i];
		}
	}
	s_timers.push_back(HighPrecisionTimeMeasurement(name));
	return s_timers[s_timers.size() - 1];
}
