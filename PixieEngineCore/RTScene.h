#ifndef PIXIE_ENGINE_RTSCENE
#define PIXIE_ENGINE_RTSCENE

#include "pch.h"
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
	glm::vec3 skyColor = glm::vec3(0);
	Primitive* rootPrimitive = nullptr;

	RTScene();
	~RTScene();

	void Update();
	bool Intersect(const Ray& ray, RTInteraction& outCollision, float tMax = Infinity) const;
	bool IntersectP(const Ray& ray, float tMax = Infinity) const;
	glm::vec3 GetSkyColor(const Ray& ray) const;

	friend class SceneLoader;
};

#endif // PIXIE_ENGINE_RTSCENE