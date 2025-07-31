#pragma once
#include "pch.h"
#include "Scene/Scene.h"

class SceneManager {
public:
	static bool Initialize();
	static void Free();
	static void LoadModel(const std::filesystem::path& filePath);

	static void Start();
	static void Pause();
	static void Continue();
	static void Stop();
	static void Update();
	static void FixedUpdate();

	static std::shared_ptr<Scene> CreateScene(const std::string& name = "New Scene");

	static SceneObject* CreateObject(const std::string& name, SceneObject* parent = nullptr, const Transform& transform = Transform());

	static void RemoveObject(const std::string& objectName);
	static void RemoveObjects(const std::string& objectName);
	static void RemoveObject(const SceneObject* object);
	static void RemoveObjects(const std::vector<SceneObject*>& objects);

	static void SetObjectParent(SceneObject* object, SceneObject* parent = nullptr);

	static SceneObject* FindObject(const std::string& objectName);
	static std::vector<SceneObject*> FindObjects(const std::string& objectName);

	static SceneObject* GetSelectedObject();
	static void SetSelectedObject(SceneObject* object);
	static void RemoveSelectedObject();

	static std::shared_ptr<Scene> GetScene();

protected:
	static std::filesystem::path m_currentScenePath;
	static std::shared_ptr<Scene> m_currentScene;
	static std::shared_ptr<Scene> m_virtualScene;
	static std::shared_ptr<Scene> m_activeScene;
	static SceneObject* m_selectedObject;
	static bool m_playing;
	static bool m_paused;
};
