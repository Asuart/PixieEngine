#include "pch.h"
#include "SceneManager.h"
#include "Resources/ModelLoader.h"
#include "Resources/TextureLoader.h"

namespace PixieEngine {

bool SceneManager::Initialize() {
	return true;
}

void SceneManager::Free() {
	m_currentScene = nullptr;
	m_activeScene = nullptr;
	m_virtualScene = nullptr;
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

}