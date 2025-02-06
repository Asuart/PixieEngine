#pragma once
#include "pch.h"
#include "SceneObject.h"
#include "BVHNode.h"
#include "RayTracing/Lights.h"
#include "RayTracing/Shapes.h"
#include "Scene/Components/Components.h"

class Scene;

struct ObjectCache {
	ObjectType type;
	int32_t index;
	Bounds3f bounds;
};

class SceneSnapshot {
public:
	SceneSnapshot(Scene* scene);
	~SceneSnapshot();

	Bounds3f GetBounds() const;
	std::vector<BVHNode>& GetNodes();
	std::vector<Light*>& GetLights();
	std::vector<DiffuseAreaLight*>& GetAreaLights();
	DiffuseAreaLight* GetAreaLight(int32_t index);
	std::vector<Light*>& GetInfiniteLights();
	std::vector<Triangle>& GetTriangles();
	std::vector<Camera>& GetCameras();
	uint32_t GetTrianglesCount();
	uint32_t GetInvalidTrianglesCount();
	uint32_t GetNodesCount();
	Material& GetMaterial(int32_t index);

	std::optional<ShapeIntersection> Intersect(const Ray& ray, int32_t* boxChecks, int32_t* shapeChecks, Float tMax = Infinity);
	bool IsIntersected(const Ray& ray, int32_t* boxChecks, int32_t* shapeChecks, Float tMax = Infinity);

private:
	std::vector<Material> m_materials;
	std::vector<Triangle> m_triangles;
	std::vector<Sphere> m_spheres;
	std::vector<ObjectBVHNode> m_objects;
	std::vector<BVHNode> m_nodes;
	std::vector<DiffuseAreaLight*> m_areaLights;
	std::vector<Light*> m_infiniteLights;
	std::vector<Light*> m_lights;
	std::vector<Camera> m_cameras;
	uint32_t m_invalidTrianglesCount;

	int32_t BuildMeshBVH(Mesh* mesh, Material* material);
	int32_t BuildObjectsBVHRecoursive(std::vector<ObjectCache>& cache, int32_t start, int32_t objectsCount);
	int32_t BuildBVHRecoursive(int32_t start, int32_t trianglesCount);
};
