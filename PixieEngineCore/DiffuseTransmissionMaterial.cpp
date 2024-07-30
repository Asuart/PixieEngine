#include "pch.h"
#include "DiffuseTransmissionMaterial.h"

DiffuseTransmissionMaterial::DiffuseTransmissionMaterial(const std::string& name, glm::vec3 reflectance, glm::vec3 transmittance, float scale)
	: RTMaterial(name), reflectance(reflectance), transmittance(transmittance), scale(scale) {}

BxDF* DiffuseTransmissionMaterial::GetBxDF(const RTInteraction& intr) const {
	return new DiffuseTransmissionBxDF(reflectance * scale, transmittance * scale);
}