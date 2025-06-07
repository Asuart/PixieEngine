#pragma once
#include "pch.h"
#include "SceneManager.h"

enum class GeneratedObject : int32_t {
	Sphere = 0,
	PointLight,
	DirectionalLight,
};

enum class GeneratedScene : int32_t {
	Empty = 0,
	DemoSphere,
	SmallAndBigSpheres,
	TestMaterials,
	RandomizedSpheres,
};

class SceneGenerator {
public:
	static void CreateScene(GeneratedScene type);
	static SceneObject* CreateObject(GeneratedObject type);

	static void CreateEmptyScene();
	static void CreateDemoSphereScene();
	static void CreateSmallAndBigSpheresScene();
	static void CreateTestMaterialsScene();
	static void CreateRandomizedSpheresScene();

	static SceneObject* CreateSphere(std::shared_ptr<Scene> scene, glm::vec3 position, float radius, std::shared_ptr<Material> material, bool active = true);
	static SceneObject* CreatePointLight(std::shared_ptr<Scene> scene, glm::vec3 position, glm::vec3 color, float strength);
	static SceneObject* CreateDirectionalLight(std::shared_ptr<Scene> scene, glm::vec3 color, float strength, Transform orientation);
	static SceneObject* CreateCamera(std::shared_ptr<Scene> scene, glm::vec3 from, glm::vec3 to, glm::vec3 up, glm::ivec2 resolution = { 1280, 720 }, float fovy = 39.6f, float near = 0.01f, float far = 100.0f);

	static std::shared_ptr<Material> CreateDiffuseMaterial(glm::vec3 color);
	static std::shared_ptr<Material> CreateGlassMaterial(float refraction = 1.7, float roughness = 0.0f);
	static std::shared_ptr<Material> CreateMetalMaterial(glm::vec3 color, float roughness = 0.0f);
};
