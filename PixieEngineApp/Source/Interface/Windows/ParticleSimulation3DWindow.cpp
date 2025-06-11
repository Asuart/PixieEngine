#include "pch.h"
#include "ParticleSimulation3DWindow.h"
#include "PixieEngineApp.h"
#include "SceneManager.h"

static const char* PARTICLE_VERTEX_SHADER_SOURCE = R"(
#version 430 core

layout(location = 0) in float position_x;
layout(location = 1) in float position_y;
layout(location = 2) in float position_z;

uniform mat4 mProjection;
uniform mat4 mView;


void main() {
    gl_Position = mProjection * mView * vec4(position_x, position_y, position_z, 1.0f);
}
)";

static const char* PARTICLE_FRAGMENT_SHADER_SOURCE = R"(
#version 430 core

out vec4 FragColor;

void main() {
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
)";

ParticleSimulation3DWindow::ParticleSimulation3DWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter), m_frameBuffer({ 1200, 1200 }), m_fittedFrameBuffer({1280, 720}) {

	InitSimulationTwoBoxes();

	glPointSize(4.0f);

	m_view = glm::lookAt(glm::vec3(-300, 300, - 300), glm::vec3(100, 100, 100), glm::vec3(0, 1, 0));
	m_projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 1000.0f);

	m_particlesProgram = ShaderManager::CompileShader(PARTICLE_VERTEX_SHADER_SOURCE, PARTICLE_FRAGMENT_SHADER_SOURCE);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void ParticleSimulation3DWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Particle Simulation", 0)) {

		GLint originalViewport[4];
		glGetIntegerv(GL_VIEWPORT, originalViewport);

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };

		for (int32_t i = 0; i < 8; i++) {
			m_simulation.Step();
		}

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		m_frameBuffer.Bind();
		m_frameBuffer.ResizeViewport();
		m_frameBuffer.Clear();

		m_particlesProgram.Bind();
		m_particlesProgram.SetUniformMat4f("mProjection", m_projection, GL_FALSE);
		m_particlesProgram.SetUniformMat4f("mView", m_view, GL_FALSE);

		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_simulation.m_gpuBuffers[(int32_t)ParticleSimulation3D::Buffer::PositionsX].id);
		glVertexAttribPointer(0, 1, GL_DOUBLE, GL_FALSE, sizeof(double), 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_simulation.m_gpuBuffers[(int32_t)ParticleSimulation3D::Buffer::PositionsY].id);
		glVertexAttribPointer(1, 1, GL_DOUBLE, GL_FALSE, sizeof(double), 0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_simulation.m_gpuBuffers[(int32_t)ParticleSimulation3D::Buffer::PositionsZ].id);
		glVertexAttribPointer(2, 1, GL_DOUBLE, GL_FALSE, sizeof(double), 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, m_simulation.m_gpuBuffers[(int32_t)ParticleSimulation3D::Buffer::Sizes].id);
		glVertexAttribPointer(3, 1, GL_DOUBLE, GL_FALSE, sizeof(double), 0);
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_POINTS, 0, m_simulation.m_numParticlesOnGPU);

		m_frameBuffer.Unbind();
		glClearColor(0.0, 0.0, 0.0, 0.0);

		m_fittedFrameBuffer.Bind();
		m_fittedFrameBuffer.Resize(glmViewportResolution);
		m_fittedFrameBuffer.ResizeViewport();
		m_fittedFrameBuffer.Clear();

		RenderEngine::DrawTextureFitted(m_frameBuffer.GetColorHandle(), m_frameBuffer.GetResolution(), glmViewportResolution);

		m_fittedFrameBuffer.Unbind();

		glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);

		ImGui::Image((void*)(uint64_t)m_fittedFrameBuffer.GetColorHandle(), viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });

	}
	ImGui::End();
}

void ParticleSimulation3DWindow::InitSimulationRandomPosition(int32_t numParticles) {
	for (int32_t i = 0; i < numParticles; i++) {
		m_simulation.CreateParticle({ RandomFloat(50, 150), RandomFloat(50, 150), RandomFloat(50, 150) }, 1.0f, 1.0f);
	}
	m_simulation.Init();
}

void ParticleSimulation3DWindow::InitSimulationRandomPositionAndSize(int32_t numParticles, float minSize, float maxSize) {
	for (int32_t i = 0; i < numParticles; i++) {
		m_simulation.CreateParticle({ RandomFloat(50, 150), RandomFloat(50, 150), RandomFloat(50, 150) }, RandomFloat(minSize, maxSize), 1);
	}
	m_simulation.Init();
}

void ParticleSimulation3DWindow::InitSimulationBox() {
	CreateBox({ 100, 100, 100 }, { 50, 50, 50 }, 5);
	m_simulation.Init();
}

void ParticleSimulation3DWindow::InitSimulationTwoBoxes() {
	CreateBox({ 100, 50, 100 }, { 50, 50, 50 }, 10);
	CreateBox({ 125, 150, 100 }, { 50, 50, 50 }, 10);
	m_simulation.Init();
}

void ParticleSimulation3DWindow::CreateBox(glm::vec3 position, glm::vec3 size, float particleSize) {
	glm::ivec3 resolution = size / particleSize;
	glm::vec3 step = size / glm::vec3(resolution);
	glm::vec3 pointCenterOffset = glm::vec3(particleSize, particleSize, particleSize) * 0.5f;
	int32_t startIndex = m_simulation.m_numParticlesOnCPU;
	for (int32_t z = 0; z < resolution.z; z++) {
		for (int32_t y = 0; y < resolution.y; y++) {
			for (int32_t x = 0; x < resolution.x; x++) {
				glm::vec3 pos = position + pointCenterOffset + glm::vec3(x, y, z) * step - size * 0.5f;
				m_simulation.CreateParticle(pos, particleSize, 1.0f);
			}
		}
	}

	for (int32_t z = 0; z < resolution.z; z++) {
		for (int32_t y = 0; y < resolution.y; y++) {
			for (int32_t x = 0; x < resolution.x; x++) {
				if (x < resolution.x - 1) {
					m_simulation.CreateSpring(
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x,
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x + 1,
						100000.0f);
				}
				if (y < resolution.y - 1) {
					m_simulation.CreateSpring(
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x,
						startIndex + z * resolution.y * resolution.x + (y + 1) * resolution.x + x,
						100000.0f);
				}
				if (z < resolution.z - 1) {
					m_simulation.CreateSpring(
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x,
						startIndex + (z + 1) * resolution.y * resolution.x + y * resolution.x + x,
						100000.0f);
				}
				if (x < resolution.x - 1 && y < resolution.y - 1) {
					m_simulation.CreateSpring(
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x,
						startIndex + z * resolution.y * resolution.x + (y + 1) * resolution.x + x + 1,
						100000.0f);
				}
				if (x < resolution.x - 1 && z < resolution.z - 1) {
					m_simulation.CreateSpring(
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x,
						startIndex + (z + 1) * resolution.y * resolution.x + y * resolution.x + x + 1,
						100000.0f);
				}
				if (y < resolution.y - 1 && z < resolution.z - 1) {
					m_simulation.CreateSpring(
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x,
						startIndex + (z + 1) * resolution.y * resolution.x + (y + 1) * resolution.x + x,
						100000.0f);
				}
				if (x < resolution.x - 1 && y < resolution.y - 1 && z < resolution.z - 1) {
					m_simulation.CreateSpring(
						startIndex + z * resolution.y * resolution.x + y * resolution.x + x,
						startIndex + (z + 1) * resolution.y * resolution.x + (y + 1) * resolution.x + x + 1,
						100000.0f);
				}
			}
		}
	}
}
