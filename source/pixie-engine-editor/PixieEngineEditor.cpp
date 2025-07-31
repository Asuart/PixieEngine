#include "pch.h"
#include "PixieEngineEditor.h"
#include "SceneManager.h"

PixieEngineEditor::PixieEngineEditor(PixieEngineMainWindow* window) : 
	m_mainWindow(window), m_interface(*this) {}

PixieEngineEditor::~PixieEngineEditor() {
}

void PixieEngineEditor::Initialize() {
	m_interface.Initialize();
}

void PixieEngineEditor::Start() {
	SceneManager::Initialize();
	SceneManager::Start();
	while (!m_mainWindow->ShouldClose()) {
		HighPrecisionTimer::StartTimer("Total Frame Time");

		Time::Update();
		UserInput::Reset();
		SceneManager::Update();
		m_interface.Draw();
		SDL_GL_SwapWindow(m_mainWindow->GetSDLWindow());
		HandleUserInput();

		HighPrecisionTimer::StopTimer("Total Frame Time");
	}
	SceneManager::Free();
}

void PixieEngineEditor::Close() {
	m_mainWindow->Close();
}

SDL_Window* PixieEngineEditor::GetSDLWindow() {
	return m_mainWindow->GetSDLWindow();
}

SDL_GLContext PixieEngineEditor::GetGLContext() {
	return m_mainWindow->GetGLContext();
}

void PixieEngineEditor::HandleWindowResize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void PixieEngineEditor::HandleUserInput() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		ImGui_ImplSDL2_ProcessEvent(&evnt);
		if (evnt.type == SDL_QUIT) {
			m_mainWindow->Close();
		}
		if (evnt.type == SDL_WINDOWEVENT && evnt.window.event == SDL_WINDOWEVENT_CLOSE && evnt.window.windowID == SDL_GetWindowID(m_mainWindow->GetSDLWindow())) {
			m_mainWindow->Close();
		}
	}
}
