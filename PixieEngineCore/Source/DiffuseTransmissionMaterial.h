#pragma once
#include "RTMaterial.h"

class DiffuseTransmissionMaterial : public RTMaterial {
public:
	DiffuseTransmissionMaterial(const std::string& name, Vec3 reflectance, Vec3 transmittance, Float scale);

	BxDF* GetBxDF(const RTInteraction& intr) const override;

private:
	Vec3 reflectance, transmittance;
	Float scale;
};
