#pragma once
#include "UID.h"
#include "Math/Transform.h"
#include "SceneObject.h"
#include "Camera.h"
#include "Skyboxes.h"
#include "Scene/Components/Components.h"

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
	const HDRISkybox& GetSkybox() const;
	void SetSkybox(const HDRISkybox& skybox);

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
	HDRISkybox m_skybox;

	friend class SceneManager;
};
