#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "ComponentRenderer.h"
#include "Physics/ParticleSimulation2D.h"

class ParticleSimulationWindow : public InterfaceWindow {
public:
	ParticleSimulationWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;

protected:
	FrameBuffer m_frameBuffer;
	FrameBuffer m_fittedFrameBuffer;
	ParticleSimulation2D m_simulation;
	GLuint m_program;
	GLuint m_vao;
	glm::mat4 m_view;
	glm::mat4 m_projection;
};
