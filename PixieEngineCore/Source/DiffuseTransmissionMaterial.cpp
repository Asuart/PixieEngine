
#include "DiffuseTransmissionMaterial.h"

DiffuseTransmissionMaterial::DiffuseTransmissionMaterial(const std::string& name, Vec3 reflectance, Vec3 transmittance, Float scale)
	: Material(name), reflectance(reflectance), transmittance(transmittance), scale(scale) {}

BxDF* DiffuseTransmissionMaterial::GetBxDF(const SurfaceInteraction& intr) const {
	return new DiffuseTransmissionBxDF(reflectance * scale, transmittance * scale);
}