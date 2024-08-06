
#include "DiffuseMaterial.h"

DiffuseMaterial::DiffuseMaterial(const std::string& name, RTTexture* texture) 
	: Material(name) {
	rtTexture = texture;
}

BxDF* DiffuseMaterial::GetBxDF(const SurfaceInteraction& intr) const {
	return new DiffuseBxDF(rtTexture->Sample(intr));
}