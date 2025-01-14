#include "pch.h"
#include "PixieEngineApp.h"
#include "OpenGLInterface.h"

PixieEngineApp::PixieEngineApp() :
	m_interface(*this) {
	ResourceManager::Initialize();
	GlobalRenderer::Initialize();
	SceneManager::Initialize();
	m_interface.Initialize();
}

PixieEngineApp::~PixieEngineApp() {
	glfwTerminate();
}

void PixieEngineApp::Start() {
	while (!m_window.IsShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Time::Update();
		UserInput::Reset();
		glfwPollEvents();
		HandleUserInput();
		SceneManager::Update();
		m_interface.Draw();
		glfwSwapBuffers(m_window.GetGLFWWindow());
	}
}

GLFWwindow* PixieEngineApp::GetGLFWWindow() {
	return m_window.GetGLFWWindow();
}

void PixieEngineApp::HandleResize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void PixieEngineApp::HandleUserInput() {}
