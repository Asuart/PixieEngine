#include "pch.h"
#include "PixieEngineApp.h"
#include "OpenGLInterface.h"

PixieEngineApp::PixieEngineApp() :
	m_interface(*this) {}

PixieEngineApp::~PixieEngineApp() {
	glfwTerminate();
}

void PixieEngineApp::Initialize() {
	ResourceManager::Initialize();
	GlobalRenderer::Initialize();
	SceneManager::Initialize();
	m_interface.Initialize();
}

void PixieEngineApp::Start() {
	while (!m_mainWindow.IsShouldClose()) {
		HighPrecisionTimer::StartTimer("Total Frame Time");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Time::Update();
		UserInput::Reset();
		glfwPollEvents();
		HandleUserInput();
		SceneManager::Update();
		m_interface.Draw();
		glfwSwapBuffers(m_mainWindow.GetGLFWWindow());
		HighPrecisionTimer::StopTimer("Total Frame Time");
	}
}

GLFWwindow* PixieEngineApp::GetGLFWWindow() {
	return m_mainWindow.GetGLFWWindow();
}

void PixieEngineApp::HandleWindowResize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void PixieEngineApp::HandleUserInput() {}
