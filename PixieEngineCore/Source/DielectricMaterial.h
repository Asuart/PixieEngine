#pragma once
#include "pch.h"
#include "RTMaterial.h"

class DielectricMaterial : public RTMaterial {
public:
	DielectricMaterial(const std::string& name, float uRoughness, float vRoughness, float eta, bool remapRoughness);

	BxDF* GetBxDF(const RTInteraction& intr) const override;

private:
	float uRoughness, vRoughness;
	bool remapRoughness;
};
