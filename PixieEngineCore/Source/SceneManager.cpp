#include "pch.h"
#include "SceneManager.h"

std::filesystem::path SceneManager::m_currentScenePath = "../Assets/Scenes/default-min.obj";
std::shared_ptr<Scene> SceneManager::m_currentScene = nullptr;
std::shared_ptr<Scene> SceneManager::m_virtualScene = nullptr;
std::shared_ptr<Scene> SceneManager::m_activeScene = nullptr;
std::shared_ptr<SceneSnapshot> SceneManager::m_sceneSnapshot = nullptr;
SceneObject* SceneManager::m_selectedObject = nullptr;

void SceneManager::Initialize() {
	m_currentScene = std::shared_ptr<Scene>(ResourceManager::LoadScene(m_currentScenePath));
	if (!m_currentScene) {
		m_currentScene = std::make_shared<Scene>("New Scene");
		m_activeScene = m_currentScene;
		SceneObject* object = ResourceManager::LoadModel(m_currentScenePath);
		if (object) {
			AttachObject(object);
		}
	}
	m_activeScene = m_currentScene;
	UpdateSceneSnapshot();
}

void SceneManager::LoadDemoScene(GeneratedScene type) {
	m_currentScene = std::shared_ptr<Scene>(SceneGenerator::CreateScene(type));
	m_activeScene = m_currentScene;
	UpdateSceneSnapshot();
}

void SceneManager::LoadScene(const std::filesystem::path& filePath) {
	std::shared_ptr<Scene> loadedScene = std::shared_ptr<Scene>(ResourceManager::LoadScene(filePath));
	if (loadedScene) {
		m_currentScenePath = filePath;
		m_currentScene = loadedScene;
		m_activeScene = loadedScene;
		UpdateSceneSnapshot();
	}
}

void SceneManager::LoadModel(const std::filesystem::path& filePath) {
	if (!m_activeScene) return;
	SceneObject* object = ResourceManager::LoadModel(filePath);
	if (object) {
		AttachObject(object);
	}
}

void SceneManager::ReloadScene() {
	std::shared_ptr<Scene> loadedScene = std::shared_ptr<Scene>(ResourceManager::LoadScene(m_currentScenePath));
	if (loadedScene) {
		m_currentScene = loadedScene;
		m_activeScene = loadedScene;
		UpdateSceneSnapshot();
	}
}

void SceneManager::Start() {

}

void SceneManager::Pause() {

}

void SceneManager::Stop() {

}

void SceneManager::Update() {

}

void SceneManager::FixedUpdate() {

}

std::shared_ptr<Scene> SceneManager::GetScene() {
	return m_activeScene;
}

std::shared_ptr<SceneSnapshot> SceneManager::GetSceneSnapshot() {
	return m_sceneSnapshot;
}

SceneObject* SceneManager::GetSelectedObject() {
	return m_selectedObject;
}

void SceneManager::SetSelectedObject(SceneObject* object) {
	m_selectedObject = object;
}

void SceneManager::UpdateSceneSnapshot() {
	if (!m_activeScene) return;
	m_sceneSnapshot = std::make_shared<SceneSnapshot>(m_activeScene.get());
}

SceneObject* SceneManager::CreateObject(const std::string& name) {
	SceneObject* object = new SceneObject(name);
	AttachObject(object);
	return object;
}

SceneObject* SceneManager::CreateObject(GeneratedObject type) {
	SceneObject* object = SceneGenerator::CreateObject(type);
	AttachObject(object);
	return object;
}

void SceneManager::RemoveSelectedObject() {
	if (!m_selectedObject || (m_activeScene && m_selectedObject == m_activeScene->GetRootObject())) {
		return;
	}

	UpdateSceneSnapshot();
}

void SceneManager::AttachObject(SceneObject* object, SceneObject* parent) {
	if (!m_activeScene) return;
	if (!parent) parent = m_activeScene->GetRootObject();
	parent->m_children.push_back(object);
	object->m_parent = parent;
	UpdateSceneSnapshot();
}
