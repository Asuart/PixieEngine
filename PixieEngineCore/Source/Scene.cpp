#include "pch.h"
#include "Scene.h"

Scene::Scene(const std::string& name) :
	m_name(name) {}

Scene::~Scene() {
	delete m_rootObject;
}

const std::string& Scene::GetName() const {
	return m_name;
}

void Scene::SetName(const std::string& name) {
	m_name = name;
}

SceneObject* Scene::FindObject(const std::string& objectName) const {
	return m_rootObject->FindObject(objectName);
}

std::vector<SceneObject*> Scene::FindObjects(const std::string& objectName) const {
	return m_rootObject->FindObjects(objectName);
}

SceneObject* Scene::FindObjectWithComponent(ComponentType type) const {
	return m_rootObject->FindObjectWithComponent(type);
}

std::vector<SceneObject*> Scene::FindObjectsWithComponent(ComponentType type) const  {
	return m_rootObject->FindObjectsWithComponent(type);
}

SceneObject* Scene::GetRootObject() const {
	return m_rootObject;
}

const std::vector<AreaLightComponent*>& Scene::GetAreaLights() const {
	return m_areaLights;
}

const std::vector<DirectionalLightComponent*>& Scene::GetDirectionalLights() const {
	return m_directionalLights;
}

const std::vector<PointLightComponent*>& Scene::GetPointLights() const {
	return m_pointLights;
}

const std::vector<CameraComponent*>& Scene::GetCameras() const {
	return m_cameras;
}

Bounds3f Scene::GetBounds() const {
	return Bounds3f();
}

void Scene::SetSkybox(Skybox* skybox) {
	m_skybox = skybox;
}

Skybox* Scene::GetSkybox() const {
	return m_skybox;
}

void Scene::Start() {
	m_rootObject->OnStart();
}

void Scene::Update() {
	m_rootObject->OnUpdate();
}

void Scene::FixedUpdate() {
	m_rootObject->OnFixedUpdate();
}
