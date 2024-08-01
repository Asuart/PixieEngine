#pragma once
#include "Textures.h"
#include "Materials.h"
#include "Primitive.h"
#include "Light.h"
#include "Camera.h"

class SceneLoader;

class RTScene {
public:
	std::vector<RTTexture*> textures;
	std::vector<RTMaterial*> materials;
	std::vector<Shape*> shapes;
	std::vector<DiffuseAreaLight> lights;
	std::vector<Camera> cameras;
	Camera* mainCamera;
	Vec3 skyColor = Vec3(0);
	Primitive* rootPrimitive = nullptr;

	RTScene();
	~RTScene();

	void Update();
	bool Intersect(const Ray& ray, RTInteraction& outCollision, Float tMax = Infinity) const;
	bool IntersectP(const Ray& ray, Float tMax = Infinity) const;
	Vec3 GetSkyColor(const Ray& ray) const;

	friend class SceneLoader;
};
