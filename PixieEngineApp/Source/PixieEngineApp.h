#pragma once
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
	RayTracingRenderer* m_rayTracingRenderer;
	SceneRenderer* m_sceneRenderer;
	glm::ivec2 m_viewportResolution;
	FrameBuffer* m_viewportFrameBuffer;
	SceneObject* m_selectedObject = nullptr;
	bool m_rayTracingViewport = true;

	void DrawUI();
	void DrawSettingsWindow();
	void DrawViewportWindow();
	void DrawSceneWindow();
	void DrawSceneTree(SceneObject* object);
	void DrawTransform(Transform& transform);
	void DrawMaterialsWindow();
	void DrawInspectorWindow();
	void UpdateViewportResolution(glm::ivec2 resolution);
	void ReloadScene();
	void HandleUserInput();

	friend class RayTracingRenderer;
};
