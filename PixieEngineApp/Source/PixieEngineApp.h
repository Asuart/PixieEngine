#pragma once
#include "PixieEngineAppHeaders.h"
#include "PixieEngineWindow.h"
#include "RayTracingRenderer.h"

class PixieEngineApp {
public:
	PixieEngineApp();
	~PixieEngineApp();

	void Start();
	GLFWwindow* GetGLFWWindow();
	void HandleResize(uint32_t width, uint32_t height);

private:
	PixieEngineWindow m_window;
	RTScene* m_rtScene;
	RayTracingRenderer* m_rayTracingRenderer;
	glm::ivec2 m_viewportResolution;
	FrameBuffer* m_viewportFrameBuffer;

	void DrawUI();
	void UpdateViewportResolution(glm::ivec2 resolution);
};
