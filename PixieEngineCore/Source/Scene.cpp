
#include "Scene.h"

Scene::Scene(const std::string& name)
	: name(name), rootObject(new SceneObject("root object")) {}

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

void Scene::Start() {}

void Scene::Update() {}

void Scene::FixedUpdate() {}