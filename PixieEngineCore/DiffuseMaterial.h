#ifndef PIXIE_ENGINE_DIFFUSE_MATERIAL
#define PIXIE_ENGINE_DIFFUSE_MATERIAL

#include "pch.h"
#include "RTMaterial.h"

class DiffuseMaterial : public RTMaterial {
public:
	DiffuseMaterial(const std::string& name, RTTexture* texture);

	BxDF* GetBxDF(const RTInteraction& intr) const override;
};

#endif // PIXIE_ENGINE_DIFFUSE_MATERIAL