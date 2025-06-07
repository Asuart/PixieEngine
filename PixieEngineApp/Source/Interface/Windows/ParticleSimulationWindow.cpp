#include "pch.h"
#include "ParticleSimulationWindow.h"
#include "PixieEngineApp.h"
#include "SceneManager.h"

const char* VERTEX_SHADER_SOURCE = R"(
#version 330 core

layout(location = 0) in vec2 pos;

uniform mat4 mProjection;
uniform mat4 mView;

void main() {
    gl_Position = mProjection * mView * vec4(pos.x, pos.y, -20.0f, 1.0f);
}
)";

const char* FRAGMENT_SHADER_SOURCE = R"(
#version 330 core

out vec4 FragColor;

void main() {
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

GLuint CompileShader(const char* vertShaderSrc, const char* fragShaderSrc) {
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int32_t logLength;

	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
	if (logLength) {
		std::cout << &vertShaderError[0] << std::endl;
	}

	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
	if (logLength) {
		std::cout << &fragShaderError[0] << std::endl;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	if (logLength) {
		std::cout << &programError[0] << std::endl;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

ParticleSimulationWindow::ParticleSimulationWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter), m_frameBuffer({ 1200, 1200 }), m_fittedFrameBuffer({1280, 720}) {
	for (int32_t i = 0; i < 256; i++) {
		m_simulation.CreateParticle({ RandomFloat(50, 150), RandomFloat(50, 150) }, 1.0f, 1.0f);
	}
	m_simulation.Init();

	glPointSize(4.0f);

	m_view = glm::mat4(1.0f);
	m_projection = glm::ortho(0.f, 200.f, 0.0f, 200.f, 0.1f, 100.0f);

	m_program = CompileShader(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);


}

void ParticleSimulationWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Particle Simulation", 0)) {

		GLint originalViewport[4];
		glGetIntegerv(GL_VIEWPORT, originalViewport);

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };

		for (int32_t i = 0; i < 10; i++) {
			m_simulation.Step();
		}

		m_frameBuffer.Bind();
		m_frameBuffer.ResizeViewport();
		m_frameBuffer.Clear();

		glUseProgram(m_program);
		glUniformMatrix4fv(glGetUniformLocation(m_program, "mProjection"), 1, GL_FALSE, &m_projection[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(m_program, "mView"), 1, GL_FALSE, &m_view[0][0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_simulation.m_positionsBuffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_POINTS, 0,m_simulation.m_numParticles);

		m_frameBuffer.Unbind();

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
