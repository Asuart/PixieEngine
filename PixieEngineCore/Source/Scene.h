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

	Scene(const std::string& name);

	void Start();
	void Update();
	void FixedUpdate();

	const std::string& GetName();
	void SetName(const std::string& name);
	void AddObject(SceneObject* object, SceneObject* parent = nullptr);
	void AddObject(SceneObject* object, const std::string& parentName);
	void RemoveObject(const std::string& objectName);
	void RemoveObjects(const std::string& objectName);
	SceneObject* FindObject(const std::string& objectName);
	std::vector<SceneObject*> FindObjects(const std::string& objectName);
	SceneObject* FindObjectWithComponent(ComponentType type);
	std::vector<SceneObject*> FindObjectsWithComponent(ComponentType type);
	const SceneObject* GetRootObject() const;
	SceneObject* GetRootObject();
	std::vector<AreaLightComponent*>& GetAreaLights();
	std::vector<DirectionalLightComponent*>& GetDirectionalLights();
	std::vector<PointLightComponent*>& GetPointLights();
	std::vector<CameraComponent*>& GetCameras();
	Bounds3f GetBounds();
	void SetSkybox(Skybox* skybox);
	Skybox* GetSkybox();

protected:
	std::string m_name;
	SceneObject* m_rootObject = nullptr;
	std::vector<CameraComponent*> m_cameras;
	std::vector<DirectionalLightComponent*> m_directionalLights;
	std::vector<PointLightComponent*> m_pointLights;
	std::vector<AreaLightComponent*> m_areaLights;
	Skybox* m_skybox = nullptr;
};
