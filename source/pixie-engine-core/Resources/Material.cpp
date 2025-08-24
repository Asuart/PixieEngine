#include "pch.h"
#include "Material.h"

namespace PixieEngine {

Material::Material(const std::string& name, glm::vec3 albedo, glm::vec3 emissionColor, float emissionStrength, float roughness, float metallic, float transparency, float refraction) :
	name(name), emissionColor(emissionColor), emissionStrength(emissionStrength), albedo(albedo), transparency(transparency), refraction(refraction), metallic(metallic), roughness(roughness) {}

}