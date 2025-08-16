#include "pch.h"
#include "Timer.h"
#include "Debug/Log.h"

namespace PixieEngine {

Timer::Timer(const std::string& name) : m_name(name) {
	m_start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {
	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
	uint64_t deltaTime = (end - m_start).count();
	Log::Message("%s: %i", m_name, deltaTime);
}

}