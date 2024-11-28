#pragma once
#include "pch.h"
#include "Component.h"
#include "MeshComponent.h"
#include "EngineTime.h"

class SoftbodyComponent : public Component {
public:
	SoftbodyComponent(SceneObject* parent);

	void OnStart() override;
	void OnUpdate() override;

protected:
	const int32_t m_stepsPerSecond = 256;
	const float m_deltaTime = 1.0f / m_stepsPerSecond;
	const glm::fvec3 m_gravity = glm::fvec3(0.0f, -9.81f, 0.0f);
	float m_timeAligner = 0.0f;
	Mesh* m_mesh = nullptr;
	std::vector<glm::fvec3> m_positions;
	std::vector<glm::fvec3> m_lastPositions;
	std::vector<glm::fvec3> m_forces;
	std::vector<int32_t> m_springStarts;
	std::vector<int32_t> m_springEnds;
	std::vector<float> m_springLengths;
	std::vector<int32_t> m_coloredSpringsCounts;

	void SetForcesToGravity();
	void UpdatePositions();
	void UpdateMesh();
};