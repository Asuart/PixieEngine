#include "pch.h"
#include "Scene.h"

Scene::Scene(const std::string& name) :
	m_name(name), m_rootObject(new SceneObject(name)) {}

const std::string& Scene::GetName() {
	return m_name;
}

void Scene::SetName(const std::string& name) {
	m_name = name;
}

void Scene::AddObject(SceneObject* object, SceneObject* parent) {
	if (PointLightComponent* pointLight = object->GetComponent<PointLightComponent>()) {
		m_pointLights.push_back(pointLight);
	}
	if (DirectionalLightComponent* pointLight = object->GetComponent<DirectionalLightComponent>()) {
		m_directionalLights.push_back(pointLight);
	}
	if (parent) {
		parent->AddChild(object);
	}
	else {
		m_rootObject->AddChild(object);
	}
}

void Scene::AddObject(SceneObject* object, const std::string& parentName) {
	SceneObject* parent = FindObject(parentName);
	if (parent == nullptr) {
		return;
	}
	AddObject(object, parent);
}

void Scene::RemoveObject(const std::string& objectName) {
	SceneObject* object = FindObject(objectName);
	if (object) {
		object->Remove();
	}
}

void Scene::RemoveObjects(const std::string& objectName) {
	std::vector<SceneObject*> objects = FindObjects(objectName);
	for (int32_t i = 0; i < objects.size(); i++) {
		objects[i]->Remove();
	}
}

SceneObject* Scene::FindObject(const std::string& objectName) {
	return m_rootObject->FindObject(objectName);
}

std::vector<SceneObject*> Scene::FindObjects(const std::string& objectName) {
	return m_rootObject->FindObjects(objectName);
}

SceneObject* Scene::FindObjectWithComponent(ComponentType type) {
	return m_rootObject->FindObjectWithComponent(type);
}

std::vector<SceneObject*> Scene::FindObjectsWithComponent(ComponentType type) {
	return m_rootObject->FindObjectsWithComponent(type);
}

const SceneObject* Scene::GetRootObject() const {
	return m_rootObject;
}

SceneObject* Scene::GetRootObject() {
	return m_rootObject;
}

std::vector<DiffuseAreaLightComponent*>& Scene::GetDiffuseAreaLights() {
	return m_diffuseAreaLights;
}

std::vector<DirectionalLightComponent*>& Scene::GetDirectionalLights() {
	return m_directionalLights;
}

std::vector<PointLightComponent*>& Scene::GetPointLights() {
	return m_pointLights;
}

std::vector<CameraComponent*>& Scene::GetCameras() {
	return m_cameras;
}

Bounds3f Scene::GetBounds() {
	return Bounds3f();
}

void Scene::SetSkybox(Skybox* skybox) {
	m_skybox = skybox;
}

Skybox* Scene::GetSkybox() {
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
