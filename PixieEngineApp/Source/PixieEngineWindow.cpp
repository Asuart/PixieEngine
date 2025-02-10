#include "pch.h"
#include "PixieEngineWindow.h"

PixieEngineWindow::PixieEngineWindow(uint32_t width, uint32_t height) {
	if (glfwInit() == 0) {
		std::cout << "GLFW initialization failed.\n";
		exit(1);
	}

	m_window = glfwCreateWindow(width, height, "Pixie Engine", NULL, NULL);
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGL()) {
		std::cout << "GLAD initialization failed.\n";
		exit(1);
	}

	UserInput::SetInputWindow(m_window);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(openglCallbackFunction, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
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
