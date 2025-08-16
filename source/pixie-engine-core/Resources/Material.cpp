#include "pch.h"
#include "Material.h"

namespace PixieEngine {

inline const glm::vec3 cDefaultAlbedoTextureColor = { 1.0f, 1.0f, 1.0f };
inline const float cDefaultRoughnessTexture = 1.0f;
inline const float cDefaultMetallicTexture = 1.0f;

Material::Material(const std::string& name, glm::vec3 albedo, glm::vec3 emissionColor, float emissionStrength, float roughness, float metallic, float transparency, float refraction) :
	m_name(name), m_emissionColor(emissionColor), m_emissionStrength(emissionStrength), m_albedo(albedo), m_transparency(transparency), m_refraction(refraction), m_metallic(metallic), m_roughness(roughness) {
	m_albedoTexture = Texture({ 1, 1 }, GL_RGB, GL_RGB, GL_FLOAT, &cDefaultAlbedoTextureColor);
	m_roughnessTexture = Texture({ 1, 1 }, GL_RED, GL_RED, GL_FLOAT, &cDefaultRoughnessTexture);
	m_metallicTexture = Texture({ 1, 1 }, GL_RED, GL_RED, GL_FLOAT, &cDefaultMetallicTexture);
	const float defaulAO = 1.0f;
	m_aoTexture = Texture({ 1, 1 }, GL_RED, GL_RED, GL_FLOAT, &defaulAO);
	const glm::vec3 defaultNormal = glm::vec3(0, 0, 1);
	m_normalTexture = Texture({ 1, 1 }, GL_RGB, GL_RGB, GL_FLOAT, &defaultNormal);
}

}