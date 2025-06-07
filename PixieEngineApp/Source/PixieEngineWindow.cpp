#include "pch.h"
#include "PixieEngineWindow.h"

PixieEngineWindow::PixieEngineWindow(uint32_t width, uint32_t height) {
	if (glfwInit() == 0) {
		Log::Error("GLFW initialization failed");
		exit(1);
	}

	m_window = glfwCreateWindow(width, height, "Pixie Engine", NULL, NULL);
	glfwMakeContextCurrent(m_window);

	UserInput::SetInputWindow(m_window);
}

PixieEngineWindow::~PixieEngineWindow() {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool PixieEngineWindow::IsShouldClose() const {
	return glfwWindowShouldClose(m_window);
}

GLFWwindow* PixieEngineWindow::GetGLFWWindow() const {
	return m_window;
}

glm::ivec2 PixieEngineWindow::GetWindowSize() const {
	glm::ivec2 resolution;
	glfwGetWindowSize(m_window, &resolution.x, &resolution.y);
	return resolution;
}
