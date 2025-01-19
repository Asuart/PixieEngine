#pragma once
#include "UID.h"
#include "Transform.h"
#include "SceneObject.h"
#include "Camera.h"
#include "MaterialComponent.h"
#include "CameraComponent.h"
#include "PointLightComponent.h"
#include "DirectionalLightComponent.h"
#include "Skyboxes.h"
#include "AreaLightComponent.h"

class Scene {
public:
	const UID id;

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
	Skybox* GetSkybox() const;
	void SetSkybox(Skybox* skybox);

	SceneObject* FindObject(const std::string& objectName) const;
	std::vector<SceneObject*> FindObjects(const std::string& objectName) const;
	SceneObject* FindObjectWithComponent(ComponentType type) const;
	std::vector<SceneObject*> FindObjectsWithComponent(ComponentType type) const;
	const std::vector<AreaLightComponent*>& GetAreaLights() const;
	const std::vector<DirectionalLightComponent*>& GetDirectionalLights() const;
	const std::vector<PointLightComponent*>& GetPointLights() const;
	const std::vector<CameraComponent*>& GetCameras() const;

protected:
	std::string m_name = "New Scene";
	SceneObject* m_rootObject = new SceneObject("root");
	std::vector<CameraComponent*> m_cameras;
	std::vector<DirectionalLightComponent*> m_directionalLights;
	std::vector<PointLightComponent*> m_pointLights;
	std::vector<AreaLightComponent*> m_areaLights;
	Skybox* m_skybox = nullptr;

	friend class SceneManager;
};
