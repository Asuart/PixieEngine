#include "pch.h"
#include "DiffuseMaterial.h"

DiffuseMaterial::DiffuseMaterial(const std::string& name, RTTexture* texture)
	: RTMaterial(name, texture) {}

BxDF* DiffuseMaterial::GetBxDF(const RTInteraction& intr) const {
	return new DiffuseBxDF(texture->Sample(intr));
}