#pragma once
#include "Material.h"

class DielectricMaterial : public Material {
public:
	DielectricMaterial(const std::string& name, Float uRoughness, Float vRoughness, Float eta, bool remapRoughness);

	BxDF* GetBxDF(const SurfaceInteraction& intr) const override;

private:
	Float uRoughness, vRoughness;
	bool remapRoughness;
};
