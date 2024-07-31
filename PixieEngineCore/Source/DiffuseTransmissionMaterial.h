#pragma once
#include "pch.h"
#include "RTMaterial.h"

class DiffuseTransmissionMaterial : public RTMaterial {
public:
	DiffuseTransmissionMaterial(const std::string& name, glm::vec3 reflectance, glm::vec3 transmittance, float scale);

	BxDF* GetBxDF(const RTInteraction& intr) const override;

private:
	glm::vec3 reflectance, transmittance;
	float scale;
};
