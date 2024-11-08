#pragma once
#include "pch.h"
#include "UserInput.h"

class PixieEngineWindow {
public:
	PixieEngineWindow(uint32_t windowWidth = 1600, uint32_t windowHeight = 900);
	~PixieEngineWindow();

	bool IsShouldClose() const;
	GLFWwindow* GetGLFWWindow() const;
	glm::ivec2 GetWindowSize() const;

private:
	GLFWwindow* m_window;
};
