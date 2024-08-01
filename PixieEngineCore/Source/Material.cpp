#include "Material.h"

Material::Material(const std::string& name, Vec3 _albedo, Vec3 _emission, Float _roughness, Float _metallic, Float _transparency, Float _eta)
	: name(name), albedo(_albedo), emission(_emission), roughness(_roughness), metallic(_metallic), transparency(_transparency), eta(_eta) {}