#pragma once
#include "Material.h"

class DiffuseMaterial : public Material {
public:
	DiffuseMaterial(const std::string& name, RTTexture* texture);

	BxDF* GetBxDF(const SurfaceInteraction& intr) const override;
};
