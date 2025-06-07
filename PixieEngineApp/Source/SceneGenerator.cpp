#include "pch.h"
#include "SceneGenerator.h"
#include "Resources/TextureGenerator.h"

void SceneGenerator::CreateScene(GeneratedScene type) {
	switch (type) {
	case GeneratedScene::Empty: CreateEmptyScene(); break;
	case GeneratedScene::DemoSphere: CreateDemoSphereScene(); break;
	case GeneratedScene::SmallAndBigSpheres: CreateSmallAndBigSpheresScene(); break;
	case GeneratedScene::TestMaterials: CreateTestMaterialsScene(); break;
	case GeneratedScene::RandomizedSpheres: CreateRandomizedSpheresScene(); break;
	default: CreateEmptyScene(); break;
	}
}

SceneObject* SceneGenerator::CreateObject(GeneratedObject type) {
	switch (type) {
	case GeneratedObject::Sphere: return CreateSphere(SceneManager::GetScene(), glm::vec3(0), 1.0f, nullptr);
	case GeneratedObject::PointLight: return CreatePointLight(SceneManager::GetScene(), glm::vec3(0), glm::vec3(1.0f), 10.0f);
	case GeneratedObject::DirectionalLight: return CreateDirectionalLight(SceneManager::GetScene(), glm::vec3(1.0f), 10.0f, Transform());
	default: return nullptr;
	}
}

void SceneGenerator::CreateEmptyScene() {
	SceneManager::CreateScene();
}

void SceneGenerator::CreateDemoSphereScene() {
	std::shared_ptr<Scene> scene = SceneManager::CreateScene("Demo Sphere");
	std::shared_ptr<Material> defaultMaterial = CreateDiffuseMaterial({ 0.8f, 0.8f, 0.8f });
	scene->SetSkybox(TextureGenerator::GradientSkybox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f)));
	CreateSphere(scene, glm::vec3(-8.0f, 0.0f, 0.0f), 0.5f, defaultMaterial);
	CreateDirectionalLight(scene, glm::vec3(1.0f), 4.0f, LookAt(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void SceneGenerator::CreateSmallAndBigSpheresScene() {
	std::shared_ptr<Scene> scene = SceneManager::CreateScene("Small And Big Spheres");
	std::shared_ptr<Material> defaultMaterial = CreateDiffuseMaterial({ 0.8f, 0.8f, 0.8f });
	scene->SetSkybox(TextureGenerator::GradientSkybox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f)));
	CreateSphere(scene, glm::vec3(-8.0f, 0.0f, 0.0f), 0.5f, defaultMaterial);
	CreateSphere(scene, glm::vec3(-8.0f, -100.5f, 0.0f), 100.0f, defaultMaterial);
	CreateDirectionalLight(scene, glm::vec3(1.0f), 2.0f, LookAt(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void SceneGenerator::CreateTestMaterialsScene() {
	std::shared_ptr<Scene> scene = SceneManager::CreateScene("Test Materials Scene");
	std::shared_ptr<Material> defaultMaterial = CreateDiffuseMaterial({ 0.8f, 0.8f, 0.8f });
	scene->SetSkybox(TextureGenerator::GradientSkybox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f)));
	CreateSphere(scene, glm::vec3(-7.0f, -100.5f, 0.0f), 100.0f, defaultMaterial);
	CreateSphere(scene, glm::vec3(-7.0f, 0.0f, 0.0f), 0.5f, defaultMaterial);
	CreateSphere(scene, glm::vec3(-7.0f, 0.0f, -1.0f), 0.5f, CreateGlassMaterial());
	CreateSphere(scene, glm::vec3(-7.0f, 0.0f, 1.0f), 0.5f, CreateMetalMaterial(glm::vec3(0.8f, 0.6f, 0.2f)));
	CreateDirectionalLight(scene, glm::vec3(1.0f), 2.0f, LookAt(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	CreatePointLight(scene, glm::vec3(-7.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 10.0f);
}

void SceneGenerator::CreateRandomizedSpheresScene() {
	std::shared_ptr<Scene> scene = SceneManager::CreateScene("Randomized Spheres Scene");
	scene->SetSkybox(TextureGenerator::GradientSkybox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f)));
	CreateDirectionalLight(scene, glm::vec3(1.0f), 2.0f, LookAt(glm::vec3(-1, 1, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	CreateSphere(scene, glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, CreateDiffuseMaterial(glm::vec3(0.5f)), false);

	for (int32_t a = -11; a < 11; a++) {
		for (int32_t b = -11; b < 11; b++) {
			float chooseMaterial = RandomFloat();
			glm::vec3 center(a + 0.9 * RandomFloat(), 0.2f, b + 0.9f * RandomFloat());
			if ((center - glm::vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
				if (chooseMaterial < 0.8f) {
					glm::vec3 albedo = RandomVector() * RandomVector();
					CreateSphere(scene, center, 0.2f, CreateDiffuseMaterial(albedo));
				}
				else if (chooseMaterial < 0.95f) {
					glm::vec3 albedo = (RandomVector() + glm::vec3(1.0f)) / 2.0f;
					float fuzz = RandomFloat(0.0f, 0.5f);
					CreateSphere(scene, center, 0.2f, CreateMetalMaterial(albedo, fuzz));
				}
				else {
					CreateSphere(scene, center, 0.2f, CreateGlassMaterial(1.5f));
				}
			}
		}
	}

	CreateSphere(scene, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, CreateGlassMaterial(1.5));
	CreateSphere(scene, glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f, CreateDiffuseMaterial(glm::vec3(0.4f, 0.2f, 0.1f)));
	CreateSphere(scene, glm::vec3(4.0f, 1.0f, 0.0f), 1.0f, CreateMetalMaterial(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f));

	CreateCamera(scene, glm::vec3(13.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), { 1280, 720 }, glm::radians(20.0f));
}

SceneObject* SceneGenerator::CreateSphere(std::shared_ptr<Scene> scene, glm::vec3 position, float radius, std::shared_ptr<Material> material, bool active) {
	static int32_t sphereCounter = 0;
	SceneObject* sphere = scene->CreateObject(std::string("Sphere ") + std::to_string(sphereCounter++), nullptr, Transform(position));
	scene->CreateComponent<SphereComponent>(sphere, radius);
	scene->CreateComponent<MaterialComponent>(sphere, material);
	scene->CreateComponent<SphereRigidbodyComponent>(sphere, radius, active);
	return sphere;
}

SceneObject* SceneGenerator::CreatePointLight(std::shared_ptr<Scene> scene, glm::vec3 position, glm::vec3 color, float strength) {
	static int32_t pointLightCounter = 0;
	SceneObject* object = scene->CreateObject(std::string("Point Light ") + std::to_string(pointLightCounter++), nullptr, Transform(position));
	scene->CreateComponent<PointLightComponent>(object, color, strength);
	return object;
}

SceneObject* SceneGenerator::CreateDirectionalLight(std::shared_ptr<Scene> scene, glm::vec3 color, float strength, Transform orientation) {
	static int32_t directionalLightCounter = 0;
	SceneObject* object = scene->CreateObject(std::string("Directional Light ") + std::to_string(directionalLightCounter++), nullptr, orientation);
	scene->CreateComponent<DirectionalLightComponent>(object, orientation.GetForward(), color, strength);
	return object;
}

SceneObject* SceneGenerator::CreateCamera(std::shared_ptr<Scene> scene, glm::vec3 from, glm::vec3 to, glm::vec3 up, glm::ivec2 resolution, float fovy, float near, float far) {
	static int32_t cameraCounter = 0;
	SceneObject* object = scene->CreateObject(std::string("Camera ") + std::to_string(cameraCounter++), nullptr, LookAt(from, to, up));
	scene->CreateComponent<CameraComponent>(object, Aspect(resolution), fovy, near, far);
	return object;
}

std::shared_ptr<Material> SceneGenerator::CreateDiffuseMaterial(glm::vec3 color) {
	static int32_t diffuseMaterialCounter = 0;
	std::shared_ptr<Material> material = std::make_shared<Material>(std::string("Diffuse Material ") + std::to_string(diffuseMaterialCounter++));
	material->m_albedo = color;
	return material;
}

std::shared_ptr<Material> SceneGenerator::CreateGlassMaterial(float refraction, float roughness) {
	static int32_t glassMaterialCounter = 0;
	std::shared_ptr<Material> material = std::make_shared<Material>(std::string("Glass Material ") + std::to_string(glassMaterialCounter++));
	material->m_transparency = 1.0f;
	material->m_refraction = refraction;
	material->m_roughness = roughness;
	return material;
}

std::shared_ptr<Material> SceneGenerator::CreateMetalMaterial(glm::vec3 color, float roughness) {
	static int32_t metalMaterialCounter = 0;
	std::shared_ptr<Material> material = std::make_shared<Material>(std::string("Metal Material ") + std::to_string(metalMaterialCounter++));
	material->m_albedo = color;
	material->m_metallic = 1.0f;
	material->m_roughness = roughness;
	return material;
}
