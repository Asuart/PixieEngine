#include "Scene.h"

Scene::Scene(const std::string& name)
	: name(name), rootObject(new SceneObject("root object")) {
	Vec3 lookfrom1(0.0f, 0.0f, 10.0f);
	Vec3 lookat1(0.0f, 0.0f, 0.0f);
	Vec3 vup1(0.0f, 1.0f, 0.0f);
	Float fov1 = glm::radians(45.0f);
	Float aspect1 = 1.0f;
	Float aperture1 = 2.0f;
	Float dist_to_focus1 = glm::length(lookfrom1 - lookat1);
	Camera cam1(lookfrom1, lookat1, vup1, fov1, aspect1, aperture1, dist_to_focus1);
	cameras.push_back(cam1);

	Vec3 lookfrom2(8.4168f, 4.6076f, 6.8823f);
	Vec3 lookat2(0.0f, 1.8042f, 0.0f);
	Vec3 vup2(0.0f, 1.0f, 0.0f);
	Float fov2 = glm::radians(15.0f);
	Float aspect2 = 1.0f;
	auto aperture2 = 0.0f;
	auto dist_to_focus2 = glm::length(lookfrom2 - lookat2);
	Camera cam2(lookfrom2, lookat2, vup2, fov2, aspect2, aperture2, dist_to_focus2);
	cameras.push_back(cam2);

	Vec3 lookfrom3(-10.0f, 0.0f, 0.0f);
	Vec3 lookat3(0.0f, 0.0f, 0.0f);
	Vec3 vup3(0.0f, 1.0f, 0.0f);
	Float fov3 = glm::radians(39.6f);
	Float aspect3 = 1.0f;
	auto aperture3 = 0.0f;
	auto dist_to_focus3 = glm::length(lookfrom3 - lookat3);
	Camera cam3(lookfrom3, lookat3, vup3, fov3, aspect3, aperture3, dist_to_focus3);
	cameras.push_back(cam3);

	mainCamera = &cameras.back();
}

void Scene::AddObject(SceneObject* object, SceneObject* parent) {
	if (parent) {
		object->parent = parent;
		parent->children.push_back(object);
	}
	else {
		object->parent = rootObject;
		rootObject->children.push_back(object);
	}
	flatObjects.push_back(object);
}

void Scene::AddObject(SceneObject* object, const std::string& parentName) {
	SceneObject* parent = FindObject(parentName);
	if (parent == nullptr) return;
	AddObject(object, parent);
}

void Scene::RemoveObject(SceneObject* object) {
	for (size_t i = 0; i < flatObjects.size(); i++) {
		if (flatObjects[i] == object) {
			SceneObject* obj = flatObjects[i];
			flatObjects.erase(flatObjects.begin() + i);
			if (!obj->parent) break;
			for (size_t j = 0; j < obj->parent->children.size(); j++) {
				if (obj->parent->children[j] == obj) {
					obj->parent->children.erase(obj->parent->children.begin() + j);
					break;
				}
			}
			break;
		}
	}
}

void Scene::RemoveObject(const std::string& objectName) {
	for (size_t i = 0; i < flatObjects.size(); i++) {
		if (flatObjects[i]->name == objectName) {
			SceneObject* obj = flatObjects[i];
			flatObjects.erase(flatObjects.begin() + i);
			if (!obj->parent) break;
			for (size_t j = 0; j < obj->parent->children.size(); j++) {
				if (obj->parent->children[j] == obj) {
					obj->parent->children.erase(obj->parent->children.begin() + j);
					break;
				}
			}
			break;
		}
	}
}

void Scene::RemoveObjects(const std::string& objectName) {
	bool objectFound = false;
	while (!objectFound) {
		objectFound = false;
		for (size_t i = 0; i < flatObjects.size(); i++) {
			if (flatObjects[i]->name == objectName) {
				SceneObject* obj = flatObjects[i];
				flatObjects.erase(flatObjects.begin() + i);
				objectFound = true;
				if (!obj->parent) break;
				for (size_t j = 0; j < obj->parent->children.size(); j++) {
					if (obj->parent->children[j] == obj) {
						obj->parent->children.erase(obj->parent->children.begin() + j);
						break;
					}
				}
				break;
			}
		}
	}
}

SceneObject* Scene::FindObject(const std::string& objectName) {
	for (size_t i = 0; i < flatObjects.size(); i++) {
		if (flatObjects[i]->name == objectName) {
			return flatObjects[i];
		}
	}
	return nullptr;
}

std::vector<SceneObject*> Scene::FindObjects(const std::string& objectName) {
	std::vector<SceneObject*> objects;
	for (size_t i = 0; i < flatObjects.size(); i++) {
		if (flatObjects[i]->name == objectName) {
			objects.push_back(flatObjects[i]);
		}
	}
	return objects;
}

SceneObject* Scene::FindObjectWithComponent(const std::string& componentName) {
	for (size_t i = 0; i < flatObjects.size(); i++) {
		if (flatObjects[i]->GetComponent(componentName)) {
			return flatObjects[i];
		}
	}
	return nullptr;
}

std::vector<SceneObject*> Scene::FindObjectsWithComponent(const std::string& componentName) {
	std::vector<SceneObject*> objects;
	for (size_t i = 0; i < flatObjects.size(); i++) {
		if (flatObjects[i]->GetComponent(componentName)) {
			objects.push_back(flatObjects[i]);
		}
	}
	return objects;
}

SceneObject* Scene::GetRootObject() {
	return rootObject;
}

std::vector<Material*>& Scene::GetMaterialsList() {
	return materials;
}

std::vector<Mesh*>& Scene::GetMeshesList() {
	return meshes;
}

std::vector<MaterialComponent*>& Scene::GetAreaLights() {
	return areaLights;
}

std::vector<Camera>& Scene::GetCameras() {
	return cameras;
}

Camera* Scene::GetMainCamera() {
	return mainCamera;
}

void Scene::SetMainCamera(uint32_t index) {
	mainCamera = &cameras[index];
}

void Scene::SetMainCamera(Camera* camera) {
	mainCamera = camera;
}

void Scene::Start() {}

void Scene::Update() {}

void Scene::FixedUpdate() {}

void Scene::MakeGeometrySnapshot() {
	if (geometrySnapshot) delete geometrySnapshot;
	geometrySnapshot = new GeometrySnapshot(flatObjects);
}

GeometrySnapshot* Scene::GetGeometrySnapshot() {
	return geometrySnapshot;
}

bool Scene::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	return geometrySnapshot->Intersect(ray, outCollision, stats, tMax);
}

bool Scene::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	return geometrySnapshot->IntersectP(ray, stats, tMax);
}

Vec3 Scene::GetSkyColor(const Ray& ray) {
	return Vec3(0.0f, 0.0f, 0.0f);
}