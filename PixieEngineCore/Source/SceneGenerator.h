#pragma once
#include "pch.h"
#include "Scene.h"

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

	static Scene* CreateScene(GeneratedScene type) {
		switch (type) {
		case GeneratedScene::Empty: return CreateEmptyScene();
		case GeneratedScene::DemoSphere: return CreateDemoSphereScene();
		case GeneratedScene::SmallAndBigSpheres: return CreateSmallAndBigSpheresScene();
		case GeneratedScene::TestMaterials: return CreateTestMaterialsScene();
		case GeneratedScene::RandomizedSpheres: return CreateRandomizedSpheresScene();
		default: return CreateEmptyScene();
		}
	}

	static SceneObject* CreateObject(GeneratedObject type) {
		switch (type) {
		case GeneratedObject::Sphere: return CreateSphere(Vec3(0), 1.0f);
		case GeneratedObject::PointLight: return CreatePointLight(Vec3(0), Vec3(1.0f), 10.0f);
		case GeneratedObject::DirectionalLight: return CreateDirectionalLight(Vec3(1.0f), 10.0f, Transform());
		default: return nullptr;
		}
	}

	static Scene* CreateEmptyScene() {
		Scene* scene = new Scene("Empty Scene");
		return scene;
	}

	static Scene* CreateDemoSphereScene() {
		Scene* scene = new Scene("Demo Sphere");
		scene->SetSkybox(new GradientColorSkybox(Vec3(1.0f, 1.0f, 1.0f), Vec3(0.5f, 0.7f, 1.0f)));
		scene->AddObject(CreateSphere(Vec3(-8.0f, 0.0f, 0.0f), 0.5f));
		scene->AddObject(CreateDirectionalLight(Vec3(1.0f), 4.0f, LookAt(Vec3(-1.0f, 1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f))));
		return scene;
	}

	static Scene* CreateSmallAndBigSpheresScene() {
		Scene* scene = new Scene("Small And Big Spheres");
		scene->SetSkybox(new GradientColorSkybox(Vec3(1.0f, 1.0f, 1.0f), Vec3(0.5f, 0.7f, 1.0f)));
		scene->AddObject(CreateSphere(Vec3(-8.0f, 0.0f, 0.0f), 0.5f));
		scene->AddObject(CreateSphere(Vec3(-8.0f, -100.5f, 0.0f), 100.0f));
		scene->AddObject(CreateDirectionalLight(Vec3(1.0f), 2.0f, LookAt(Vec3(-1.0f, 1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f))));
		return scene;
	}

	static Scene* CreateTestMaterialsScene() {
		Scene* scene = new Scene("Test Materials Scene");
		scene->SetSkybox(new GradientColorSkybox(Vec3(1.0f, 1.0f, 1.0f), Vec3(0.5f, 0.7f, 1.0f)));
		scene->AddObject(CreateSphere(Vec3(-7.0f, -100.5f, 0.0f), 100.0f));
		scene->AddObject(CreateSphere(Vec3(-7.0f, 0.0f, 0.0f), 0.5f));
		scene->AddObject(CreateSphere(Vec3(-7.0f, 0.0f, -1.0f), 0.5f, CreateGlassMaterial()));
		scene->AddObject(CreateSphere(Vec3(-7.0f, 0.0f, 1.0f), 0.5f, CreateMetalMaterial(Vec3(0.8f, 0.6f, 0.2f))));
		scene->AddObject(CreateDirectionalLight(Vec3(1.0f), 2.0f, LookAt(Vec3(-1.0f, 1.0f, -1.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f))));
		scene->AddObject(CreatePointLight(Vec3(-7.0f, 2.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), 10.0f));
		return scene;
	}

	static Scene* CreateRandomizedSpheresScene() {
		Scene* scene = new Scene("Randomized Spheres Scene");
		scene->SetSkybox(new GradientColorSkybox(Vec3(1.0f, 1.0f, 1.0f), Vec3(0.5f, 0.7f, 1.0f)));
		scene->AddObject(CreateDirectionalLight(Vec3(1.0f), 2.0f, LookAt(Vec3(-1, 1, -1), Vec3(0, 0, 0), Vec3(0, 1, 0))));

		scene->AddObject(CreateSphere(Vec3(0.0f, -1000.0f, 0.0f), 1000.0f, CreateDiffuseMaterial(Vec3(0.5f))));

		for (int32_t a = -11; a < 11; a++) {
			for (int32_t b = -11; b < 11; b++) {
				Float chooseMaterial = RandomFloat();
				Vec3 center(a + 0.9 * RandomFloat(), 0.2f, b + 0.9f * RandomFloat());
				if ((center - Vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
					if (chooseMaterial < 0.8f) {
						Vec3 albedo = RandomVector() * RandomVector();
						scene->AddObject(CreateSphere(center, 0.2f, CreateDiffuseMaterial(albedo)));
					}
					else if (chooseMaterial < 0.95f) {
						Vec3 albedo = (RandomVector() + Vec3(1.0f)) / 2.0f;
						Float fuzz = RandomFloat(0.0f, 0.5f);
						scene->AddObject(CreateSphere(center, 0.2f, CreateMetalMaterial(albedo, fuzz)));
					}
					else {
						scene->AddObject(CreateSphere(center, 0.2f, CreateGlassMaterial(1.5f)));
					}
				}
			}
		}

		scene->AddObject(CreateSphere(Vec3(0.0f, 1.0f, 0.0f), 1.0f, CreateGlassMaterial(1.5)));
		scene->AddObject(CreateSphere(Vec3(-4.0f, 1.0f, 0.0f), 1.0f, CreateDiffuseMaterial(Vec3(0.4f, 0.2f, 0.1f))));
		scene->AddObject(CreateSphere(Vec3(4.0f, 1.0f, 0.0f), 1.0f, CreateMetalMaterial(Vec3(0.7f, 0.6f, 0.5f), 0.0f)));

		scene->AddObject(CreateCamera(Vec3(13.0f, 2.0f, 3.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), {1280, 720}, glm::radians(20.0f)));

		return scene;
	}

	static SceneObject* CreateSphere(Vec3 position, Float radius, Material* material = nullptr) {
		static int32_t sphereCounter = 0;
		SceneObject* sphere = new SceneObject(std::string("Sphere ") + std::to_string(sphereCounter++), Transform(position));
		SphereComponent* sphereComponent = new SphereComponent(sphere, radius);
		sphere->AddComponent(sphereComponent);
		MaterialComponent* materialComponent = new MaterialComponent(material ? material : ResourceManager::GetDefaultMaterial(), sphere);
		sphere->AddComponent(materialComponent);
		return sphere;
	}

	static SceneObject* CreatePointLight(Vec3 position, Vec3 color, Float strength) {
		static int32_t pointLightCounter = 0;
		SceneObject* object = new SceneObject(std::string("Point Light ") + std::to_string(pointLightCounter++), Transform(position));
		PointLightComponent* pointLight = new PointLightComponent(object, color, strength);
		object->AddComponent(pointLight);
		return object;
	}

	static SceneObject* CreateDirectionalLight(Vec3 color, Float strength, Transform orientation) {
		static int32_t directionalLightCounter = 0;
		SceneObject* object = new SceneObject(std::string("Directional Light ") + std::to_string(directionalLightCounter++), orientation);
		DirectionalLightComponent* directionalLight = new DirectionalLightComponent(object, orientation.GetForward(), color, strength);
		object->AddComponent(directionalLight);
		return object;
	}

	static SceneObject* CreateCamera(Vec3 from, Vec3 to, Vec3 up, glm::ivec2 resolution = { 1280, 720 }, Float fovy = 39.6f) {
		static int32_t cameraCounter = 0;
		Transform t = LookAt(from, to, up);
		SceneObject* object = new SceneObject(std::string("Camera ") + std::to_string(cameraCounter++), t);
		CameraComponent* camera = new CameraComponent(object, resolution, fovy, 0.01f, 100.0f);
		object->AddComponent(camera);
		return object;
	}

	static Material* CreateDiffuseMaterial(Vec3 color) {
		static int32_t diffuseMaterialCounter = 0;
		Material material(std::string("Diffuse Material ") + std::to_string(diffuseMaterialCounter++));
		material.m_albedo = color;
		return ResourceManager::AddMaterial(material);
	}

	static Material* CreateGlassMaterial(Float refraction = 1.7, Float roughness = 0.0f) {
		static int32_t glassMaterialCounter = 0;
		Material material(std::string("Glass Material ") + std::to_string(glassMaterialCounter++));
		material.m_transparency = 1.0f;
		material.m_refraction = refraction;
		material.m_roughness = roughness;
		return ResourceManager::AddMaterial(material);
	}

	static Material* CreateMetalMaterial(Vec3 color, Float roughness = 0.0f) {
		static int32_t metalMaterialCounter = 0;
		Material material(std::string("Metal Material ") + std::to_string(metalMaterialCounter++));
		material.m_albedo = color;
		material.m_metallic = 1.0f;
		material.m_roughness = roughness;
		return ResourceManager::AddMaterial(material);
	}
};
