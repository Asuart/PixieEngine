#pragma once
#include "PixieEngineAppHeaders.h"
#include "UserInput.h"

class PixieEngineWindow {
public:
	PixieEngineWindow(uint32_t windowWidth = 1280, uint32_t windowHeight = 720);
	~PixieEngineWindow();

	bool IsShouldClose() const;
	GLFWwindow* GetGLFWWindow() const;
	glm::ivec2 GetWindowSize() const;

private:
	GLFWwindow* m_window;
};