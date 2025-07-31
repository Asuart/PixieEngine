#pragma once 
#include "pch.h"
#include "Log.h"
#include "UserInput.h"

class PixieEngineMainWindow {
public:
	PixieEngineMainWindow(const std::string& name, glm::ivec2 resolution);
	~PixieEngineMainWindow();

	SDL_Window* GetSDLWindow();
	SDL_GLContext GetGLContext();
	const std::string& GetName() const;
	glm::ivec2 GetResolution() const;
	void Close();
	bool ShouldClose() const;

protected:
	std::string m_name;
	glm::ivec2 m_resolution;
	SDL_Window* m_window;
	SDL_GLContext m_glContext;
	bool m_shouldClose = false;
};