#include "pch.h"
#include "EngineTime.h"

const Float Time::fixedDeltaTime = 0.002f;
Float Time::deltaTime = 1.0f / 60.0f;
std::chrono::milliseconds Time::lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

void Time::Update() {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	deltaTime = (newTime - lastTime).count() / 1000.0f;
	lastTime = newTime;
}

HightPrecisionTimer::HightPrecisionTimer(const std::string& name) : m_name(name) {
	m_start = std::chrono::high_resolution_clock::now();
}

HightPrecisionTimer::~HightPrecisionTimer() {
	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::cout << m_name << ": " << (end - m_start) << "\n";
}
