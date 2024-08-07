#include "RTScene.h"

RTScene* RTScene::FromScene(Scene* scene) {
	uint64_t trianglesCount = 0, invalidTrianglesCount = 0;

	RTScene* rtScene = new RTScene();
	std::vector<Primitive*> shapePrimitives;
	for (size_t sceneObjectIndex = 0; sceneObjectIndex < scene->flatObjects.size(); sceneObjectIndex++) {
		SceneObject* object = scene->flatObjects[sceneObjectIndex];

		MeshComponent* meshComopnent = object->GetComponent<MeshComponent>();
		MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>();
		if (!meshComopnent || !materialComponent) continue;

		const Mesh* mesh = meshComopnent->GetMesh();
		const Material* material = materialComponent->GetMaterial();
		if (!mesh || !material) continue;

		for (size_t i = 0; i < mesh->indices.size(); i += 3) {
			Vec3 p0 = mesh->vertices[mesh->indices[i + 0]].position;
			Vec3 p1 = mesh->vertices[mesh->indices[i + 1]].position;
			Vec3 p2 = mesh->vertices[mesh->indices[i + 2]].position;

			Shape* triangle = new CachedTriangle(p0, p1, p2);
			trianglesCount++;
			if (!triangle->IsValid()) {
				delete triangle;
				invalidTrianglesCount++;
				continue;
			}

			rtScene->shapes.push_back(triangle);
			shapePrimitives.push_back(new ShapePrimitive(triangle, material));
			if (material->emission != glm::vec3(0.0f)) {
				rtScene->lights.push_back(DiffuseAreaLight(triangle, material->emission * 10.0f));
			}
		}
	}

	rtScene->rootPrimitive = new BVHAggregate(shapePrimitives, 4);

	std::cout << "Scene triangles count: " << trianglesCount << ", invalid triangles count: " << invalidTrianglesCount << "\n";

	return rtScene;
}

RTScene::RTScene() {
	materials.push_back(new Material("Default Materail",Vec3(0.8, 0.8, 0.0)));

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
}

RTScene::~RTScene() {
	for (RTTexture* texture : textures) {
		delete texture;
	}
	for (Material* material : materials) {
		delete material;
	}
	for (Shape* shape : shapes) {
		delete shape;
	}
	delete rootPrimitive;
}

void RTScene::Update() {}

bool RTScene::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	return rootPrimitive->Intersect(ray, outCollision, stats, tMax);
}

bool RTScene::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	return rootPrimitive->IntersectP(ray, stats, tMax);
}

Vec3 RTScene::GetSkyColor(const Ray& ray) const {
	return skyColor;
}