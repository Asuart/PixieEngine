#pragma once
#include "pch.h"
#include "Texture.h"

struct Material {
	std::string m_name;
	glm::vec3 m_albedo;
	Texture m_albedoTexture;
	Texture m_normalTexture;
	float m_metallic = 0.0f;
	Texture m_metallicTexture;
	float m_roughness = 1.0f;
	Texture m_roughnessTexture;
	Texture m_aoTexture;
	glm::vec3 m_emissionColor;
	float m_emissionStrength = 0.0f;
	float m_refraction = 1.0f;
	float m_transparency = 0.0f;

	Material(const std::string& name = "Unnamed Material", glm::vec3 albedo = glm::vec3(0.8f), glm::vec3 emissionColor = glm::vec3(1.0f), float emissionStrength = 0.0f, float roughness = 1.0f, float metallic = 0.0f, float transparency = 0.0f, float refraction = 1.0f);
};
