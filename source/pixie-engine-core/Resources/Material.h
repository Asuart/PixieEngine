#pragma once
#include "pch.h"
#include "Rendering/TextureHandle.h"

namespace PixieEngine {

struct Material {
	std::string name;
	glm::vec3 albedo;
	TextureHandle albedoTexture;
	TextureHandle normalTexture;
	float metallic = 0.0f;
	TextureHandle metallicTexture;
	float roughness = 1.0f;
	TextureHandle roughnessTexture;
	TextureHandle aoTexture;
	glm::vec3 emissionColor;
	float emissionStrength = 0.0f;
	float refraction = 1.0f;
	float transparency = 0.0f;

	Material(const std::string& name = "Unnamed Material", glm::vec3 albedo = glm::vec3(0.8f), glm::vec3 emissionColor = glm::vec3(1.0f), float emissionStrength = 0.0f, float roughness = 1.0f, float metallic = 0.0f, float transparency = 0.0f, float refraction = 1.0f);
};

}