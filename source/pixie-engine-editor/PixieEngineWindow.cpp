#include "pch.h"
#include "PixieEngineWindow.h"

PixieEngineWindow::PixieEngineWindow(uint32_t width, uint32_t height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Log::Error("SDL2 initialization failed.");
        exit(1);
    }

    m_window = SDL_CreateWindow("PixieEngine Editor", 0, 0, width, height, SDL_WINDOW_OPENGL);
    if (!m_window) {
        Log::Error("SDL2 failed to create a window.");
        exit(2);
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        Log::Error("SDL2 failed to initialize OpenGL context.");
        exit(3);
    }

	UserInput::SetInputWindow(m_window);
}

PixieEngineWindow::~PixieEngineWindow() {
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void PixieEngineWindow::Close() {
    m_shouldClose = true;
}

bool PixieEngineWindow::IsShouldClose() const {
	return m_shouldClose;
}

SDL_Window* PixieEngineWindow::GetSDLWindow() const {
	return m_window;
}

glm::ivec2 PixieEngineWindow::GetWindowSize() const {
	glm::ivec2 resolution;
    SDL_GetWindowSize(m_window, &resolution.x, &resolution.y);
	return resolution;
}
