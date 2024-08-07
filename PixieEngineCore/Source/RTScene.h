#pragma once
#include "Textures.h"
#include "Material.h"
#include "Primitive.h"
#include "Light.h"
#include "Camera.h"
#include "RayTracingStatistics.h"
#include "Scene.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"

class SceneLoader;

class RTScene {
public:
	std::vector<RTTexture*> textures;
	std::vector<Material*> materials;
	std::vector<Shape*> shapes;
	std::vector<AreaLight*> lights;
	std::vector<Camera> cameras;
	Camera* mainCamera;
	Vec3 skyColor = Vec3(0);
	Primitive* rootPrimitive = nullptr;

	static RTScene* FromScene(Scene* scene);

	RTScene();
	~RTScene();

	void Update();
	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax = Infinity) const;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity) const;
	Vec3 GetSkyColor(const Ray& ray) const;

	friend class Scene;
	friend class SceneLoader;
};
