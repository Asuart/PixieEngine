#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Texture.h"

class Material {
public:
	const std::string name;
	glm::fvec3 albedo;
	glm::fvec3 emission;
	float roughness;
	float metallic;
	float transparency;
	float eta;
	Texture<glm::fvec3>* albedoTexture = nullptr;
	Texture<glm::fvec3>* normalMap = nullptr;
	Texture<float>* bumpMap = nullptr;

	Material(const std::string& name, Vec3 _albedo = Vec3(0.8f), Vec3 _emission = Vec3(0.0f), Float _roughness = 1.0f, Float _metallic = 0.0f, Float _transparency = 0.0f, Float _eta = 1.0f);
};
