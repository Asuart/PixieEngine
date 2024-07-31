#pragma once
#include "pch.h"
#include "RTMaterial.h"

class DiffuseMaterial : public RTMaterial {
public:
	DiffuseMaterial(const std::string& name, RTTexture* texture);

	BxDF* GetBxDF(const RTInteraction& intr) const override;
};
