#include "pch.h"
#include "SceneManager.h"
#include "Resources/ModelLoader.h"
#include "Resources/TextureLoader.h"

std::filesystem::path SceneManager::m_currentScenePath;
std::shared_ptr<Scene> SceneManager::m_currentScene;
std::shared_ptr<Scene> SceneManager::m_virtualScene;
std::shared_ptr<Scene> SceneManager::m_activeScene;
SceneObject* SceneManager::m_selectedObject;
bool SceneManager::m_playing;
bool SceneManager::m_paused;

bool SceneManager::Initialize() {
	m_currentScenePath = "../Assets/Scenes/default-min.obj";
	m_virtualScene = nullptr;
	m_selectedObject = nullptr;

	m_currentScene = std::make_shared<Scene>("New Scene");
	m_activeScene = m_currentScene;

	m_activeScene->m_skybox = TextureLoader::LoadSkybox("kloppenheim_01_puresky_4k.hdr");


	LoadModel(m_currentScenePath);

	return true;
}

void SceneManager::Free() {
	m_currentScene = nullptr;
	m_activeScene = nullptr;
	m_virtualScene = nullptr;
	m_selectedObject = nullptr;
}

void SceneManager::LoadModel(const std::filesystem::path& filePath) {
	if (!m_activeScene) return;
	ModelLoader::LoadModel(m_activeScene, filePath);
}

void SceneManager::Start() {
	if (m_playing) return;
	m_playing = true;
	m_activeScene->Start();
}

void SceneManager::Pause() {
	m_paused = true;
}

void SceneManager::Continue() {
	m_paused = false;
}

void SceneManager::Stop() {
	m_playing = false;
	m_paused = false;
}

void SceneManager::Update() {
	if (!m_playing || m_paused) return;
	m_activeScene->Update();
}

void SceneManager::FixedUpdate() {
	if (!m_playing || m_paused) return;
	m_activeScene->FixedUpdate();
}

std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& name) {
	if (m_playing) Stop();
	m_currentScene = std::make_shared<Scene>(name);
	m_activeScene = m_currentScene;
	return m_activeScene;
}

std::shared_ptr<Scene> SceneManager::GetScene() {
	return m_activeScene;
}

SceneObject* SceneManager::GetSelectedObject() {
	return m_selectedObject;
}

void SceneManager::SetSelectedObject(SceneObject* object) {
	m_selectedObject = object;
}

void SceneManager::RemoveSelectedObject() {
	if (!m_selectedObject || !m_activeScene || m_selectedObject == m_activeScene->GetRootObject()) {
		return;
	}
	for (size_t i = 0; i < m_selectedObject->m_parent->m_children.size(); i++) {
		if (m_selectedObject->m_parent->m_children[i] == m_selectedObject) {
			m_selectedObject->m_parent->m_children.erase(m_selectedObject->m_parent->m_children.begin() + i);
			break;
		}
	}
	std::queue<SceneObject*> objectsList;
	objectsList.push(m_selectedObject);
	m_selectedObject = nullptr;
	while (objectsList.size() > 0) {
		SceneObject* obj = objectsList.front();
		objectsList.pop();
		for (size_t i = 0; i < obj->m_children.size(); i++) {
			objectsList.push(obj->m_children[i]);
		}
		while (obj->m_components.size() > 0) {
			m_activeScene->RemoveComponent(obj->m_components.back());
		}
		delete obj;
	}
}
