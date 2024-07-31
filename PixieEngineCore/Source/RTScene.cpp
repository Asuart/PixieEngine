
#include "RTScene.h"

RTScene::RTScene() {
	materials.push_back(new DiffuseMaterial("Default Materail", new ColorTexture(glm::vec3(0.8, 0.8, 0.0))));

	glm::vec3 lookfrom1(0, 0, 10);
	glm::vec3 lookat1(0, 0, 0);
	glm::vec3 vup1(0, 1, 0);
	float fov1 = glm::radians(45.0);
	float aspect1 = 1.0;
	float aperture1 = 2.0;
	float dist_to_focus1 = glm::length(lookfrom1 - lookat1);
	Camera cam1(lookfrom1, lookat1, vup1, fov1, aspect1, aperture1, dist_to_focus1);
	cameras.push_back(cam1);

	glm::vec3 lookfrom2(8.4168, 4.6076, 6.8823);
	glm::vec3 lookat2(0, 1.8042, 0);
	glm::vec3 vup2(0, 1, 0);
	float fov2 = glm::radians(15.0);
	float aspect2 = 1.0;
	auto aperture2 = 0.0;
	auto dist_to_focus2 = glm::length(lookfrom2 - lookat2);
	Camera cam2(lookfrom2, lookat2, vup2, fov2, aspect2, aperture2, dist_to_focus2);
	cameras.push_back(cam2);

	glm::vec3 lookfrom3(-10, 0, 0);
	glm::vec3 lookat3(0, 0, 0);
	glm::vec3 vup3(0, 1, 0);
	float fov3 = glm::radians(39.6);
	float aspect3 = 1.0;
	auto aperture3 = 0.0;
	auto dist_to_focus3 = glm::length(lookfrom3 - lookat3);
	Camera cam3(lookfrom3, lookat3, vup3, fov3, aspect3, aperture3, dist_to_focus3);
	cameras.push_back(cam3);

	mainCamera = &cameras.back();
}

RTScene::~RTScene() {
	for (RTTexture* texture : textures) {
		delete texture;
	}
	for (RTMaterial* material : materials) {
		delete material;
	}
	for (Shape* shape : shapes) {
		delete shape;
	}
	delete rootPrimitive;
}

void RTScene::Update() {}

bool RTScene::Intersect(const Ray& ray, RTInteraction& outCollision, float tMax) const {
	return rootPrimitive->Intersect(ray, outCollision, tMax);
}

bool RTScene::IntersectP(const Ray& ray, float tMax) const {
	return rootPrimitive->IntersectP(ray, tMax);
}

glm::vec3 RTScene::GetSkyColor(const Ray& ray) const {
	return skyColor;
}