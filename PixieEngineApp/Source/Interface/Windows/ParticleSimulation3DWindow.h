#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "ComponentRenderer.h"
#include "Physics/ParticleSimulation3D.h"

class ParticleSimulation3DWindow : public InterfaceWindow {
public:
	ParticleSimulation3DWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;

protected:
	FrameBuffer m_frameBuffer;
	FrameBuffer m_fittedFrameBuffer;
	ParticleSimulation3D m_simulation;
	Shader m_particlesProgram;
	GLuint m_vao;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	void InitSimulationRandomPosition(int32_t numParticles);
	void InitSimulationRandomPositionAndSize(int32_t numParticles, float minSize, float maxSize);
	void InitSimulationBox();
	void InitSimulationTwoBoxes();

	void CreateBox(glm::vec3 position, glm::vec3 size, float particleSize);
};
