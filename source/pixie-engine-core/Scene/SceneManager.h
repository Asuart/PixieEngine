#pragma once
#include "pch.h"
#include "Scene/Scene.h"

namespace PixieEngine {

class SceneManager {
public:
	static bool Initialize();
	static void Free();

	static std::shared_ptr<Scene> GetScene();
	static std::shared_ptr<Scene> CreateScene(const std::string& name);

	static void Start();
	static void Pause();
	static void Continue();
	static void Stop();
	static void Update();
	static void FixedUpdate();

protected:
	inline static std::shared_ptr<Scene> m_currentScene = nullptr;
	inline static std::shared_ptr<Scene> m_virtualScene = nullptr;
	inline static std::shared_ptr<Scene> m_activeScene = nullptr;
	inline static bool m_playing = false;
	inline static bool m_paused = false;
};

}