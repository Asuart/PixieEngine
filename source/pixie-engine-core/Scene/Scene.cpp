#include "pch.h"
#include "Scene.h"

namespace PixieEngine {

Scene::Scene(const std::string& name) :
	m_name(name) {
}

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

SceneObject* Scene::GetRootObject() const {
	return m_rootObject;
}

const std::vector<DiffuseAreaLightComponent*>& Scene::GetAreaLights() const {
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

void Scene::SetSkybox(const Skybox& skybox) {
	m_skybox = skybox;
}

const Skybox& Scene::GetSkybox() const {
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

SceneObject* Scene::CreateObject(const std::string& name, SceneObject* parent, const Transform& transform) {
	if (!parent) parent = m_rootObject;
	SceneObject* object = new SceneObject(name, transform);
	object->m_parent = parent;
	parent->m_children.push_back(object);
	return object;
}

void Scene::RemoveObject(const std::string& objectName) {
	SceneObject* object = FindObject(objectName);
	RemoveObject(object);
}

void Scene::RemoveObjects(const std::string& objectName) {
	std::vector<SceneObject*> objects = FindObjects(objectName);
	RemoveObjects(objects);
}

void Scene::RemoveObject(const SceneObject* object) {
	if (object == m_rootObject) return;
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

void Scene::RemoveObjects(const std::vector<SceneObject*>& objects) {
	for (int32_t i = 0; i < objects.size(); i++) {
		RemoveObject(objects[i]);
	}
}

void Scene::SetObjectParent(SceneObject* object, SceneObject* parent) {
	if (!parent || object == m_rootObject) return;
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

void Scene::RemoveComponent(Component* component) {
	if (!component || !component->m_parent) return;
	if (component->m_type == ComponentType::PointLight) {
		for (size_t i = 0; i < m_pointLights.size(); i++) {
			if (m_pointLights[i] == component) {
				m_pointLights.erase(m_pointLights.begin() + i);
			}
		}
	}
	else if (component->m_type == ComponentType::DiffuseAreaLight) {
		for (size_t i = 0; i < m_areaLights.size(); i++) {
			if (m_areaLights[i] == component) {
				m_areaLights.erase(m_areaLights.begin() + i);
			}
		}
	}
	else if (component->m_type == ComponentType::DirectionalLight) {
		for (size_t i = 0; i < m_directionalLights.size(); i++) {
			if (m_directionalLights[i] == component) {
				m_directionalLights.erase(m_directionalLights.begin() + i);
			}
		}
	}
	else if (component->m_type == ComponentType::Camera) {
		for (size_t i = 0; i < m_cameras.size(); i++) {
			if (m_cameras[i] == component) {
				m_cameras.erase(m_cameras.begin() + i);
			}
		}
	}
	for (int32_t i = 0; i < component->m_parent->m_components.size(); i++) {
		if (component->m_parent->m_components[i] == component) {
			component->m_parent->m_components.erase(component->m_parent->m_components.begin() + i);
			break;
		}
	}
	delete component;
}

}