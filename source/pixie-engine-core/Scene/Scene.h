#pragma once
#include "Math/Transform.h"
#include "SceneObject.h"
#include "Camera.h"
#include "Skybox.h"
#include "Scene/Components/Components.h"

class Scene {
public:
	Scene() = default;
	Scene(const std::string& name);
	~Scene();

	void Start();
	void Update();
	void FixedUpdate();

	const std::string& GetName() const;
	void SetName(const std::string& name);
	SceneObject* GetRootObject() const;
	Bounds3f GetBounds() const;
	const Skybox& GetSkybox() const;
	void SetSkybox(const Skybox& skybox);

	void RemoveObject(const std::string& objectName);
	void RemoveObjects(const std::string& objectName);
	void RemoveObject(const SceneObject* object);
	void RemoveObjects(const std::vector<SceneObject*>& objects);
	void SetObjectParent(SceneObject* object, SceneObject* parent = nullptr);
	void RemoveComponent(Component* component);
	SceneObject* CreateObject(const std::string& name, SceneObject* parent = nullptr, const Transform& transform = Transform());
	SceneObject* FindObject(const std::string& objectName) const;
	std::vector<SceneObject*> FindObjects(const std::string& objectName) const;
	const std::vector<DiffuseAreaLightComponent*>& GetAreaLights() const;
	const std::vector<DirectionalLightComponent*>& GetDirectionalLights() const;
	const std::vector<PointLightComponent*>& GetPointLights() const;
	const std::vector<CameraComponent*>& GetCameras() const;

protected:
	std::string m_name = "New Scene";
	SceneObject* m_rootObject = new SceneObject("root");
	std::vector<SceneObject*> m_flatObjects;
	std::vector<Component*> m_flatComponents;
	std::vector<CameraComponent*> m_cameras;
	std::vector<DirectionalLightComponent*> m_directionalLights;
	std::vector<PointLightComponent*> m_pointLights;
	std::vector<DiffuseAreaLightComponent*> m_areaLights;
	Skybox m_skybox;

public:
	template<typename T>
	const SceneObject* FindObjectWithComponent() const {
		return m_rootObject->FindObjectWithComponent<T>();
	}

	template<typename T>
	std::vector<const SceneObject*> FindObjectsWithComponent() const {
		std::vector<const SceneObject*> objects;
		m_rootObject->FindObjectsWithComponent<T>(objects);
		return objects;
	}

	template <class T, class... Args>
	T* CreateComponent(SceneObject* parent, Args&&... args) {
		T* component = new T(parent, args...);
		parent->m_components.push_back(component);
		if constexpr (std::is_same<T, PointLightComponent>::value) {
			m_pointLights.push_back(component);
		}
		else if constexpr (std::is_same<T, DiffuseAreaLightComponent>::value) {
			m_areaLights.push_back(component);
		}
		else if constexpr (std::is_same<T, DirectionalLightComponent>::value) {
			m_directionalLights.push_back(component);
		}
		else if constexpr (std::is_same<T, CameraComponent>::value) {
			m_cameras.push_back(component);
		}
		return component;
	}

	friend class SceneManager;
};
