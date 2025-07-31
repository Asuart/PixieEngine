#include "pch.h"
#include "PixieEngineMainWindow.h"

PixieEngineMainWindow::PixieEngineMainWindow(const std::string& name, glm::ivec2 resolution) :
	m_name(name), m_resolution(resolution) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        Log::Error("SDL2 initialization failed.");
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    m_window = SDL_CreateWindow(m_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_resolution.x, m_resolution.y, SDL_WINDOW_OPENGL);
    if (!m_window) {
        Log::Error("SDL2 failed to create a window.");
        exit(2);
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        Log::Error("SDL2 failed to initialize OpenGL context.");
        exit(3);
    }
    SDL_GL_MakeCurrent(m_window, m_glContext);

    UserInput::SetInputWindow(m_window);
}

PixieEngineMainWindow::~PixieEngineMainWindow() {
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

SDL_Window* PixieEngineMainWindow::GetSDLWindow() {
    return m_window;
}

SDL_GLContext PixieEngineMainWindow::GetGLContext() {
    return m_glContext;
}

const std::string& PixieEngineMainWindow::GetName() const {
    return m_name;
}

glm::ivec2 PixieEngineMainWindow::GetResolution() const {
    return m_resolution;
}

void PixieEngineMainWindow::Close() {
	m_shouldClose = true;
}

bool PixieEngineMainWindow::ShouldClose() const {
	return m_shouldClose;
}