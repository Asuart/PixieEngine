#include "pch.h"
#include "SceneManager.h"

std::filesystem::path SceneManager::m_currentScenePath = "../Assets/Scenes/default-min.obj";
std::shared_ptr<Scene> SceneManager::m_currentScene = nullptr;
std::shared_ptr<Scene> SceneManager::m_virtualScene = nullptr;
std::shared_ptr<Scene> SceneManager::m_activeScene = nullptr;
std::shared_ptr<SceneSnapshot> SceneManager::m_sceneSnapshot = nullptr;
SceneObject* SceneManager::m_selectedObject = nullptr;
bool SceneManager::m_playing = false;
bool SceneManager::m_paused = false;

void SceneManager::Initialize() {
	m_currentScene = std::shared_ptr<Scene>(ResourceManager::LoadScene(m_currentScenePath));
	if (!m_currentScene) {
		m_currentScene = std::make_shared<Scene>("New Scene");
		m_activeScene = m_currentScene;
		ResourceManager::LoadModel(m_currentScenePath);
	}
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
	if (!object) {
		return;
	}
	m_activeScene->m_rootObject->m_children.push_back(object);
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

SceneObject* SceneManager::CreateObject(const std::string& name, SceneObject* parent, const Transform& transform) {
	if (!parent) parent = m_activeScene->m_rootObject;
	SceneObject* object = new SceneObject(name, transform);
	object->m_parent = parent;
	parent->m_children.push_back(object);
	return object;
}

void SceneManager::RemoveObject(const std::string& objectName) {
	SceneObject* object = FindObject(objectName);
	RemoveObject(object);
}

void SceneManager::RemoveObjects(const std::string& objectName) {
	std::vector<SceneObject*> objects = FindObjects(objectName);
	RemoveObjects(objects);
}

void SceneManager::RemoveObject(const SceneObject* object) {
	if (object == m_currentScene->m_rootObject || object == m_virtualScene->m_rootObject) return;
	if (object->m_parent) {
		for (int32_t i = 0; i < object->m_parent->m_children.size(); i++) {
			if (object->m_parent->m_children[i] == object) {
				object->m_parent->m_children.erase(object->m_parent->m_children.begin() + i);
				break;
			}
		}
	}
	delete object;
}

void SceneManager::RemoveObjects(const std::vector<SceneObject*>& objects) {
	for (int32_t i = 0; i < objects.size(); i++) {
		RemoveObject(objects[i]);
	}
}

void SceneManager::SetObjectParent(SceneObject* object, SceneObject* parent) {
	if (!parent) return;
	if (object->m_parent) {
		for (int32_t i = 0; i < object->m_parent->m_children.size(); i++) {
			if (object->m_parent->m_children[i] == object) {
				object->m_parent->m_children.erase(object->m_parent->m_children.begin() + i);
				break;
			}
		}
	}
	object->m_parent = parent;
	parent->m_children.push_back(object);
}

SceneObject* SceneManager::FindObject(const std::string& objectName) {
	return m_activeScene->FindObject(objectName);
}

std::vector<SceneObject*> SceneManager::FindObjects(const std::string& objectName) {
	return m_activeScene->FindObjects(objectName);
}

SceneObject* SceneManager::FindObjectWithComponent(ComponentType type) {
	return m_activeScene->FindObjectWithComponent(type);
}

std::vector<SceneObject*> SceneManager::FindObjectsWithComponent(ComponentType type) {
	return m_activeScene->FindObjectsWithComponent(type);
}

void SceneManager::RemoveComponent(Component* component) {
	if (!component || !component->m_parent) return;
	for (int32_t i = 0; i < component->m_parent->m_components.size(); i++) {
		if (component->m_parent->m_components[i] == component) {
			component->m_parent->m_components.erase(component->m_parent->m_components.begin() + i);
			break;
		}
	}
	delete component;
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

void SceneManager::RemoveSelectedObject() {
	if (!m_selectedObject || (m_activeScene && m_selectedObject == m_activeScene->GetRootObject())) {
		return;
	}
	UpdateSceneSnapshot();
}
