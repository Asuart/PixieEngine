#pragma once
#include "PixieEngineWindow.h"
#include "PixieEngineInterface.h"
#include "ResourceManager.h"

class PixieEngineInterface;

class PixieEngineApp {
public:
	PixieEngineApp();
	~PixieEngineApp();

	void Start();
	GLFWwindow* GetGLFWWindow();
	void HandleResize(uint32_t width, uint32_t height);
	void ResetRayTracers();
	Scene* GetScene();
	SceneSnapshot* GetSceneSnapshot();
	const SceneObject* GetSelectedObject() const;
	SceneObject* GetSelectedObject();
	void SelectObject(SceneObject* object);
	void RemoveSelectedObject();
	void RestoreViewportSize();
	DefaultRenderer* GetDefaultRenderer();
	const std::filesystem::path& GetAssetsPath();
	void SetAssetsPath(const std::filesystem::path& path);

	void LoadScene(const std::filesystem::path& filePath);
	void LoadModel(const std::filesystem::path& filePath);

protected:
	std::filesystem::path m_currentScenePath = "../Assets/Scenes/untitled.glb";
	std::filesystem::path m_assetsPath = "../Assets/Scenes";
	PixieEngineWindow m_window;
	PixieEngineInterface m_interface;
	Scene* m_scene;
	SceneSnapshot* m_sceneSnapsot;
	DefaultRenderer* m_defaultRenderer;
	SceneObject* m_selectedObject = nullptr;

	void ReloadScene();
	void HandleUserInput();

	friend class RayTracingRenderer;
	friend class PixieEngineInterface;
};
