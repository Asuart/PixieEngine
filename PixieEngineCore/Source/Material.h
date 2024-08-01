#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Texture.h"

class Material {
public:
	const std::string name;
	Vec3 albedo;
	Vec3 emission;
	Float roughness;
	Float metallic;
	Float transparency;
	Float eta;
	Texture<Vec3>* albedoTexture = nullptr;
	Texture<Vec3>* normalMap = nullptr;
	Texture<Float>* bumpMap = nullptr;

	Material(const std::string& name, Vec3 _albedo = Vec3(0.8f), Vec3 _emission = Vec3(0.0f), Float _roughness = 1.0f, Float _metallic = 0.0f, Float _transparency = 0.0f, Float _eta = 1.0f);
};
