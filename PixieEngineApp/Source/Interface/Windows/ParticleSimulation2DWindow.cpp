#include "pch.h"
#include "ParticleSimulation2DWindow.h"
#include "PixieEngineApp.h"
#include "SceneManager.h"

static const char* PARTICLE_VERTEX_SHADER_SOURCE = R"(
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in float size;

uniform mat4 mProjection;
uniform mat4 mView;

out float vSize;

out VS_OUT {
    float size;
} vs_out;

void main() {
	vs_out.size = size / 100.0f;
    gl_Position = mProjection * mView * vec4(pos.x, pos.y, -20.0f, 1.0f);
}
)";

static const char* PARTICLE_GEOMETRY_SHADER_SOURCE = R"(
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    float size;
} gs_in[];

out vec2 texCoords; 

void build_quad(vec4 position) {    
	float size = gs_in[0].size;
    gl_Position = position + vec4(-size, -size, 0.0, 0.0);    // 1:bottom-left
	texCoords = vec2(-1, -1);
    EmitVertex();   
    gl_Position = position + vec4( size, -size, 0.0, 0.0);    // 2:bottom-right
	texCoords = vec2(1, -1);
    EmitVertex();
    gl_Position = position + vec4(-size,  size, 0.0, 0.0);    // 3:top-left
	texCoords = vec2(-1, 1);
    EmitVertex();
    gl_Position = position + vec4( size,  size, 0.0, 0.0);    // 4:top-right
	texCoords = vec2(1, 1);
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_quad(gl_in[0].gl_Position);
}  
)";

static const char* PARTICLE_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

in vec2 texCoords; 

out vec4 FragColor;

vec3 lerp(vec3 v0, vec3 v1, float t) {
	return v0 * (1.0 - t) + v1 * t;
}

void main() {
	float d = length(texCoords);
	if(d > 1.0f) discard;
	vec3 centerColor = vec3(0.2, 0.2, 0.2);
	vec3 edgeColor = vec3(1.0, 1.0, 1.0);
	FragColor = vec4(lerp(centerColor, edgeColor, d), 1.0);
}
)";

static const char* CELL_VERTEX_SHADER_SOURCE = R"(
#version 330 core

layout(location = 0) in vec3 pos;

uniform vec2 uOffset;
uniform vec2 uSize;

void main() {
    gl_Position = vec4(pos.x * uSize.x + uOffset.x, pos.y * uSize.y + uOffset.y, pos.z, 1.0);
}
)";

static const char* CELL_FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

out vec4 FragColor;

void main() {
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

ParticleSimulation2DWindow::ParticleSimulation2DWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter), m_frameBuffer({ 1200, 1200 }), m_fittedFrameBuffer({1280, 720}) {

	InitSimulationTwoBoxes();

	glPointSize(4.0f);

	m_view = glm::mat4(1.0f);
	m_projection = glm::ortho(0.f, 200.f, 0.0f, 200.f, 0.1f, 100.0f);

	m_particlesProgram = ShaderManager::CompileShader(PARTICLE_VERTEX_SHADER_SOURCE, PARTICLE_FRAGMENT_SHADER_SOURCE, PARTICLE_GEOMETRY_SHADER_SOURCE);
	m_cellProgram = ShaderManager::CompileShader(CELL_VERTEX_SHADER_SOURCE, CELL_FRAGMENT_SHADER_SOURCE);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
}

void ParticleSimulation2DWindow::Draw() {
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
		glBindBuffer(GL_ARRAY_BUFFER, m_simulation.m_positionsBuffer);
		glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(glm::dvec2), 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_simulation.m_sizesBuffer);
		glVertexAttribPointer(1, 1, GL_DOUBLE, GL_FALSE, sizeof(double), 0);
		glEnableVertexAttribArray(1);
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

void ParticleSimulation2DWindow::InitSimulationRandomPosition(int32_t numParticles) {
	for (int32_t i = 0; i < numParticles; i++) {
		m_simulation.CreateParticle({ RandomFloat(50, 150), RandomFloat(50, 150) }, 1.0f, 1.0f);
	}
	m_simulation.Init();
}

void ParticleSimulation2DWindow::InitSimulationRandomPositionAndSize(int32_t numParticles, float minSize, float maxSize) {
	for (int32_t i = 0; i < numParticles; i++) {
		m_simulation.CreateParticle({ RandomFloat(50, 150), RandomFloat(50, 150) }, RandomFloat(minSize, maxSize), 1.0f);
	}
	m_simulation.Init();
}

void ParticleSimulation2DWindow::InitSimulationBox() {
	CreateBox({ 100.0f, 100.0f }, { 50.0f, 50.0f }, 5.0f);
	m_simulation.Init();
}

void ParticleSimulation2DWindow::InitSimulationTwoBoxes() {
	CreateBox({ 100.0f, 50.0f }, { 50.0f, 50.0f }, 2.0f);
	CreateBox({ 125.0f, 150.0f }, { 50.0f, 50.0f }, 2.0f);
	m_simulation.Init();
}

void ParticleSimulation2DWindow::CreateBox(glm::vec2 position, glm::vec2 size, float particleSize) {
	glm::ivec2 resolution = size / particleSize;
	glm::vec2 step = size / glm::vec2(resolution);
	glm::vec2 pointCenterOffset = glm::vec2(particleSize, particleSize) * 0.5f;
	int32_t startIndex = m_simulation.m_positions.size();
	for (int32_t y = 0; y < resolution.y; y++) {
		for (int32_t x = 0; x < resolution.x; x++) {
			glm::vec2 pos = position + pointCenterOffset + glm::vec2(x, y) * step - size * 0.5f;
			m_simulation.CreateParticle(pos, particleSize, 1.0f);
		}
	}

	for (int32_t y = 0; y < resolution.y; y++) {
		for (int32_t x = 0; x < resolution.x; x++) {
			if (x < resolution.x - 1) {
				m_simulation.CreateSpring(startIndex + y * resolution.x + x, startIndex + y * resolution.x + x + 1, 1000000.0f);
			}
			if (y < resolution.y - 1) {
				m_simulation.CreateSpring(startIndex + y * resolution.x + x, startIndex + (y + 1) * resolution.x + x, 1000000.0f);
			}
			if (x < resolution.x - 1 && y < resolution.y - 1) {
				m_simulation.CreateSpring(startIndex + y * resolution.x + x, startIndex + (y + 1) * resolution.x + x + 1, 1000000.0f);
			}
		}
	}
}
