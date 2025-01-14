#pragma once
#include "PixieEngineWindow.h"
#include "Interface.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "GlobalRenderer.h"

class PixieEngineApp {
public:
	PixieEngineApp();
	~PixieEngineApp();

	void Start();
	GLFWwindow* GetGLFWWindow();
	void HandleResize(uint32_t width, uint32_t height);

protected:
	PixieEngineWindow m_window;
	Interface m_interface;

	void HandleUserInput();

	friend class RayTracingRenderer;
	friend class Interface;
};
