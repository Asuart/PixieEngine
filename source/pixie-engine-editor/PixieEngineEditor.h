#pragma once
#include "Interface/Interface.h"
#include "PixieEngineMainWindow.h"
#include "Resources/TextureLoader.h"

class PixieEngineEditor {
public:
	PixieEngineEditor(PixieEngineMainWindow* window);
	~PixieEngineEditor();

	void Initialize();
	void Start();
	void Close();
	SDL_Window* GetSDLWindow();
	SDL_GLContext GetGLContext();
	void HandleWindowResize(uint32_t width, uint32_t height);

protected:
	PixieEngineMainWindow* m_mainWindow;
	Interface m_interface;

	void HandleUserInput();

	friend class Interface;
};
