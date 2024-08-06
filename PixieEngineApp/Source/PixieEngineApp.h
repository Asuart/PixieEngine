#pragma once
#include "PixieEngineAppHeaders.h"
#include "PixieEngineWindow.h"
#include "RayTracingRenderer.h"

class RayTracingRenderer;

class PixieEngineApp {
public:
	PixieEngineApp();
	~PixieEngineApp();

	void Start();
	GLFWwindow* GetGLFWWindow();
	void HandleResize(uint32_t width, uint32_t height);

protected:
	static const uint32_t m_maxScenePathLength = 1024;
	char m_scenePath[m_maxScenePathLength] = "../Scenes/default.obj";
	PixieEngineWindow m_window;
	Scene* m_scene;
	RTScene* m_rtScene;
	RayTracingRenderer* m_rayTracingRenderer;
	glm::ivec2 m_viewportResolution;
	FrameBuffer* m_viewportFrameBuffer;

	void DrawUI();
	void UpdateViewportResolution(glm::ivec2 resolution);
	void ReloadScene();

	friend class RayTracingRenderer;
};
