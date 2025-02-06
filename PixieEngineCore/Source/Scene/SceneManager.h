#pragma once
#include "pch.h"
#include "Scene.h"
#include "SceneSnapshot.h"

class SceneManager {
public:
	static void Initialize();
	static void LoadScene(const std::filesystem::path& filePath);
	static void LoadModel(const std::filesystem::path& filePath);
	static void ReloadScene();

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
	static SceneObject* FindObjectWithComponent(ComponentType type);
	static std::vector<SceneObject*> FindObjectsWithComponent(ComponentType type);

	static void RemoveComponent(Component* component);

	static SceneObject* GetSelectedObject();
	static void SetSelectedObject(SceneObject* object);
	static void RemoveSelectedObject();

	static std::shared_ptr<Scene> GetScene();
	static std::shared_ptr<SceneSnapshot> GetSceneSnapshot();
	static void UpdateSceneSnapshot();

protected:
	static std::filesystem::path m_currentScenePath;
	static std::shared_ptr<Scene> m_currentScene;
	static std::shared_ptr<Scene> m_virtualScene;
	static std::shared_ptr<Scene> m_activeScene;
	static std::shared_ptr<SceneSnapshot> m_sceneSnapshot;
	static SceneObject* m_selectedObject;
	static bool m_playing;
	static bool m_paused;

public:
	template <class T, class... Args>
	static inline T* CreateComponent(SceneObject* parent, Args&&... args) {
		T* component = new T(parent, args...);
		parent->m_components.push_back(component);
		if constexpr (std::is_same<T, PointLightComponent>::value) {
			m_activeScene->m_pointLights.push_back(component);
		}
		else if constexpr (std::is_same<T, AreaLightComponent>::value) {
			m_activeScene->m_areaLights.push_back(component);
		}
		else if constexpr (std::is_same<T, DirectionalLightComponent>::value) {
			m_activeScene->m_directionalLights.push_back(component);
		}
		return component;
	}
};
