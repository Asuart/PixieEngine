#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Texture.h"

class Material {
public:
	const std::string name;
	glm::vec3 albedo;
	glm::vec3 emission;
	float roughness;
	float metallic;
	float transparency;
	float eta;
	Texture<glm::vec3>* albedoTexture = nullptr;
	Texture<glm::vec3>* normalMap = nullptr;
	Texture<float>* bumpMap = nullptr;

	Material(const std::string& name, glm::vec3 _albedo = glm::vec3(0.8), glm::vec3 _emission = glm::vec3(0), float _roughness = 1.0, float _metallic = 0.0, float _transparency = 0.0, float _eta = 1.0);
};
