#pragma once
#include "PixieEngineWindow.h"
#include "Interface.h"

class PixieEngineApp {
public:
	PixieEngineApp();
	~PixieEngineApp();

	void Initialize();
	void Start();
	GLFWwindow* GetGLFWWindow();
	void HandleWindowResize(uint32_t width, uint32_t height);

protected:
	PixieEngineWindow m_mainWindow;
	Interface m_interface;

	void HandleUserInput();

	friend class RayTracingRenderer;
	friend class Interface;
};
