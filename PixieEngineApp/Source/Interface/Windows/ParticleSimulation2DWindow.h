#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "ComponentRenderer.h"
#include "Physics/ParticleSimulation2D.h"

class ParticleSimulation2DWindow : public InterfaceWindow {
public:
	ParticleSimulation2DWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;

protected:
	FrameBuffer m_frameBuffer;
	FrameBuffer m_fittedFrameBuffer;
	ParticleSimulation2D m_simulation;
	Shader m_particlesProgram;
	Shader m_cellProgram;
	GLuint m_vao;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	void InitSimulationRandomPosition(int32_t numParticles);
	void InitSimulationRandomPositionAndSize(int32_t numParticles, float minSize, float maxSize);
	void InitSimulationBox();
	void InitSimulationTwoBoxes();

	void CreateBox(glm::vec2 position, glm::vec2 size, float particleSize);
};
