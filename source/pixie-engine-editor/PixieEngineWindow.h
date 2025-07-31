#pragma once
#include "pch.h"
#include "UserInput.h"

class PixieEngineWindow {
public:
	PixieEngineWindow(uint32_t windowWidth = 1600, uint32_t windowHeight = 900);
	~PixieEngineWindow();

	void Close();
	bool IsShouldClose() const;
	SDL_Window* GetSDLWindow() const;
	glm::ivec2 GetWindowSize() const;

private:
	SDL_Window* m_window;
	SDL_GLContext m_glContext;
	bool m_shouldClose = false;
};
