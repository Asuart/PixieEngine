#include "pch.h"
#include "EngineTime.h"
#include "Debug/Log.h"

namespace PixieEngine {

void Time::Update() {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	deltaTime = (newTime - lastTime).count() / 1000.0f;
	lastTime = newTime;
}

}