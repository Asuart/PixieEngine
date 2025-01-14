#pragma once
#include "pch.h"
#include "Scene.h"
#include "SceneSnapshot.h"
#include "SceneGenerator.h"

class SceneManager {
public:
	static void Initialize();
	static void LoadDemoScene(GeneratedScene type);
	static void LoadScene(const std::filesystem::path& filePath);
	static void LoadModel(const std::filesystem::path& filePath);
	static void ReloadScene();

	static void Start();
	static void Pause();
	static void Stop();
	static void Update();
	static void FixedUpdate();

	static std::shared_ptr<Scene> GetScene();
	static std::shared_ptr<SceneSnapshot> GetSceneSnapshot();
	static SceneObject* GetSelectedObject();
	static void SetSelectedObject(SceneObject* object);
	static SceneObject* CreateObject(const std::string& name);
	static SceneObject* CreateObject(GeneratedObject type);
	static void AttachObject(SceneObject* object, SceneObject* parent = nullptr);
	static void RemoveSelectedObject();

	static void UpdateSceneSnapshot();

protected:
	static std::filesystem::path m_currentScenePath;
	static std::shared_ptr<Scene> m_currentScene;
	static std::shared_ptr<Scene> m_virtualScene;
	static std::shared_ptr<Scene> m_activeScene;
	static std::shared_ptr<SceneSnapshot> m_sceneSnapshot;
	static SceneObject* m_selectedObject;

	std::vector<SceneObject> m_objects;
	std::vector<Component*> m_components;
	std::vector<uint32_t> m_activeObjects;
	std::vector<uint32_t> m_objectGaps;

public:
	template <class T, class... Args>
	inline T* CreateComponent(SceneObject* parent, Args&&... args) {
		T* component = new T(args);
		m_components.push_back(component);
		parent->m_components.push_back(component);
		return component;
	}
};
