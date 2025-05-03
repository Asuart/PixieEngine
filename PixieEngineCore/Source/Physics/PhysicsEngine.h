#pragma once
#include "pch.h"

class PhysicsEngine {
public:
	static void Initialize();
	static void Free();
	static void Update();

	static uint32_t CreateSphereBody(::Vec3 position, Float radius, bool activate);
	static ::Vec3 GetBodyPosition(uint32_t id);

protected:
	static constexpr Float cTimeStep = 1.0f / 60.0f;
	static constexpr uint32_t cMaxBodies = 1024;
	static constexpr uint32_t cNumBodyMutexes = 0;
	static constexpr uint32_t cMaxBodyPairs = 1024;
	static constexpr uint32_t cMaxContactConstraints = 1024;
	static Float m_timeAlign;
};
