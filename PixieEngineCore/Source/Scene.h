#pragma once
#include "UID.h"
#include "Transform.h"
#include "SceneObject.h"
#include "Camera.h"
#include "MaterialComponent.h"
#include "GeometrySnapshot.h"

class Scene {
public:
	const UID id;
	const std::string name;

	Scene(const std::string& name);

	void AddObject(SceneObject* object, SceneObject* parent = nullptr);
	void AddObject(SceneObject* object, const std::string& parentName);
	void RemoveObject(SceneObject* object);
	void RemoveObject(const std::string& objectName);
	void RemoveObjects(const std::string& objectName);
	SceneObject* FindObject(const std::string& objectName);
	std::vector<SceneObject*> FindObjects(const std::string& objectName);
	SceneObject* FindObjectWithComponent(const std::string& componentName);
	std::vector<SceneObject*> FindObjectsWithComponent(const std::string& componentName);
	SceneObject* GetRootObject();
	std::vector<Material*>& GetMaterialsList();
	std::vector<Mesh*>& GetMeshesList();
	std::vector<MaterialComponent*>& GetAreaLights();
	std::vector<Camera>& GetCameras();
	Camera* GetMainCamera();
	void SetMainCamera(uint32_t index);
	void SetMainCamera(Camera* camera);

	// Ray tracing finctionality
	void MakeGeometrySnapshot();
	GeometrySnapshot* GetGeometrySnapshot();
	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax = Infinity) const;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity) const;
	Vec3 GetSkyColor(const Ray& ray);

protected:
	SceneObject* rootObject = nullptr;
	std::vector<SceneObject*> flatObjects;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	std::vector<MaterialComponent*> areaLights;
	std::vector<Camera> cameras;
	Camera* mainCamera = nullptr;
	GeometrySnapshot* geometrySnapshot = nullptr;

	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();

	friend class RTScene;
	friend class SceneLoader;
};
