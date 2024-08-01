#pragma once
#include "RTMaterial.h"

class DielectricMaterial : public RTMaterial {
public:
	DielectricMaterial(const std::string& name, Float uRoughness, Float vRoughness, Float eta, bool remapRoughness);

	BxDF* GetBxDF(const RTInteraction& intr) const override;

private:
	Float uRoughness, vRoughness;
	bool remapRoughness;
};
