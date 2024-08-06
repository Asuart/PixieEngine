#pragma once
#include "Material.h"

class DiffuseTransmissionMaterial : public Material {
public:
	DiffuseTransmissionMaterial(const std::string& name, Vec3 reflectance, Vec3 transmittance, Float scale);

	BxDF* GetBxDF(const SurfaceInteraction& intr) const override;

private:
	Vec3 reflectance, transmittance;
	Float scale;
};
