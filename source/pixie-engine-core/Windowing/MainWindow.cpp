#include "pch.h"
#include "MainWindow.h"

namespace PixieEngine {

MainWindow::MainWindow(const std::string& name, glm::ivec2 resolution) :
    m_name(name), m_resolution(resolution) {}

void MainWindow::HandleEvent(const Event& event) {
    if (event.sdlEvent.type == SDL_QUIT) {
        Close();
    }
    if (event.sdlEvent.type == SDL_WINDOWEVENT && event.sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE && event.sdlEvent.window.windowID == SDL_GetWindowID(m_window)) {
    	Close();
    }
}

void MainWindow::SwapBuffers() {
    SDL_GL_SwapWindow(m_window);
}

const std::string& MainWindow::GetName() const {
    return m_name;
}

glm::ivec2 MainWindow::GetResolution() const {
    return m_resolution;
}

SDL_Window* MainWindow::GetSDLWindow() const {
    return m_window;
}

void MainWindow::Close() {
    m_shouldClose = true;
}

bool MainWindow::ShouldClose() const {
    return m_shouldClose;
}

std::optional<Event> MainWindow::PollEvent() const {
    SDL_Event evnt;
    if (SDL_PollEvent(&evnt)) {
        return Event(evnt);
    }
    return {};
}

}