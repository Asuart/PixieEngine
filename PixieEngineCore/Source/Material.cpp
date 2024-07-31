#include "Material.h"

Material::Material(const std::string& name, glm::vec3 _albedo, glm::vec3 _emission, float _roughness, float _metallic, float _transparency, float _eta)
	: name(name), albedo(_albedo), emission(_emission), roughness(_roughness), metallic(_metallic), transparency(_transparency), eta(_eta) {}