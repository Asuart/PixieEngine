#pragma once
#include "pch.h"

class PhysicsEngine {
public:
	static bool Initialize();
	static void Free();
	static void Update();

	static uint32_t CreateSphereBody(glm::vec3 position, float radius, bool activate);
	static glm::vec3 GetBodyPosition(uint32_t id);
	static void DestroyBody(uint32_t id);

protected:
	static constexpr float cTimeStep = 1.0f / 60.0f;
	static constexpr uint32_t cMaxBodies = 1024;
	static constexpr uint32_t cNumBodyMutexes = 0;
	static constexpr uint32_t cMaxBodyPairs = 1024;
	static constexpr uint32_t cMaxContactConstraints = 1024;
	static float m_timeAlign;
};
