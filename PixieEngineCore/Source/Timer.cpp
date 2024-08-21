#include "pch.h"
#include "Timer.h"

const Float Timer::fixedDeltaTime = 0.002f;
Float Timer::deltaTime = 1.0f / 60.0f;
std::chrono::milliseconds Timer::lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

void Timer::Update() {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	deltaTime = (newTime - lastTime).count() / 1000.0f;
	lastTime = newTime;
}