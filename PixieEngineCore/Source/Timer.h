#pragma once
#include "pch.h"

class Timer {
public:
	static const Float fixedDeltaTime;
	static Float deltaTime;
	static std::chrono::milliseconds lastTime;

	static void Update();
};