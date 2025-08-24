#pragma once 
#include "pch.h"
#include "SDL.h"
#include "Debug/Log.h"
#include "UserInput/UserInput.h"
#include "Event.h"
#include "Rendering/RenderEngine.h"

namespace PixieEngine {

class MainWindow {
public:
	MainWindow() = delete;
	virtual ~MainWindow() {};

	void HandleEvent(const Event& event);
	void SwapBuffers();

	const std::string& GetName() const;
	glm::ivec2 GetResolution() const;
	SDL_Window* GetSDLWindow() const;
	void Close();
	bool ShouldClose() const;
	std::optional<Event> PollEvent() const;

protected:
	std::string m_name;
	glm::ivec2 m_resolution;
	SDL_Window* m_window;
	bool m_shouldClose = false;

	MainWindow(const std::string& name, glm::ivec2 resolution);
};

}