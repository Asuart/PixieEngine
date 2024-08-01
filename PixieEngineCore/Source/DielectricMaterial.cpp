
#include "DielectricMaterial.h"

DielectricMaterial::DielectricMaterial(const std::string& name, Float uRoughness, Float vRoughness, Float _eta, bool remapRoughness)
	: RTMaterial(name), uRoughness(uRoughness), vRoughness(vRoughness), remapRoughness(remapRoughness) {
	eta = _eta;
}

BxDF* DielectricMaterial::GetBxDF(const RTInteraction& intr) const {
	Float urough = uRoughness, vrough = vRoughness;
	if (false) {
		urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
		vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
	}
	TrowbridgeReitzDistribution distrib(urough, vrough);
	return new DielectricBxDF(eta, distrib);
}