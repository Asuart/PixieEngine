#include "pch.h"
#include "Scene.h"

Scene::Scene(const std::string& name)
	: name(name), rootObject(new SceneObject("Scene")) {
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

	pointLights.push_back(new PointLight(Spectrum(1.0f, 1.0f, 1.0f), 2.0f, glm::translate(Vec3(0, 0, 0)), nullptr));
}

void Scene::AddObject(SceneObject* object, SceneObject* parent) {
	if (parent) {
		parent->AddChild(object);
	}
	else {
		rootObject->AddChild(object);
	}
}

void Scene::AddObject(SceneObject* object, const std::string& parentName) {
	SceneObject* parent = FindObject(parentName);
	if (parent == nullptr) return;
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
	return rootObject->FindObject(objectName);
}

std::vector<SceneObject*> Scene::FindObjects(const std::string& objectName) {
	return rootObject->FindObjects(objectName);
}

SceneObject* Scene::FindObjectWithComponent(const std::string& componentName) {
	return rootObject->FindObjectWithComponent(componentName);
}

std::vector<SceneObject*> Scene::FindObjectsWithComponent(const std::string& componentName) {
	return rootObject->FindObjectsWithComponent(componentName);
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

std::vector<Light*>& Scene::GetInfiniteLights() {
	return infiniteLights;
}

std::vector<PointLight*>& Scene::GetPointLights() {
	return pointLights;
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

Bounds3f Scene::GetBounds() {
	return geometrySnapshot->GetBounds();
}

void Scene::Start() {}

void Scene::Update() {}

void Scene::FixedUpdate() {}

void Scene::MakeGeometrySnapshot() {
	if (geometrySnapshot) delete geometrySnapshot;
	geometrySnapshot = new GeometrySnapshot(rootObject);
}

GeometrySnapshot* Scene::GetGeometrySnapshot() {
	return geometrySnapshot;
}

std::optional<ShapeIntersection> Scene::Intersect(const Ray& ray, Float tMax) const {
	return geometrySnapshot->Intersect(ray, tMax);
}

bool Scene::IntersectP(const Ray& ray, Float tMax) const {
	return geometrySnapshot->IntersectP(ray, tMax);
}

Vec3 Scene::GetSkyColor(const Ray& ray) {
	return Vec3(0.0f, 0.0f, 0.0f);
}