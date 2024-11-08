#pragma once
#include "pch.h"
#include "Scene.h"

//class SceneManager {
//public:
//	void Initialize();
//
//	Scene* GetScene();
//	SceneSnapshot* GetSceneSnapshot();
//	void MakeSceneSnapshot();
//	SceneObject* CreateObject(const std::string& name);
//	void AttachObject(SceneObject* object, SceneObject* parent);
//	void AttachObjectToRoot(SceneObject* object);
//
//protected:
//	Scene m_scene;
//	Scene m_activeScene;
//	SceneSnapshot* m_sceneSnapshot;
//	std::vector<SceneObject> m_objects;
//	std::vector<Component*> m_components;
//	std::vector<uint32_t> m_activeObjects;
//	std::vector<uint32_t> m_objectGaps;
//
//public:
//	template <typename T>
//	T* CreateComponent(SceneObject* parent, ...) {
//		T* component = new T(parent, ...);
//		m_components.push_back(component);
//		parent->m_components.push_back(component);
//		return component;
//	}
//};
