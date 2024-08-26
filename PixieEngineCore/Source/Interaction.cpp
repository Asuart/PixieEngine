#include "pch.h"
#include "Interaction.h"
#include "Light.h"
#include "Material.h"
#include "Camera.h"

Interaction::Interaction(Vec3 position, Vec3 normal, Vec2 uv)
	: position(position), normal(normal), uv(uv) {}

Interaction::Interaction(Vec3 position, const MediumInterface* mediumInterface)
	: position(position), mediumInterface(mediumInterface) {}

const Medium* Interaction::GetMedium(Vec3 direction) const {
	if (mediumInterface) return glm::dot(direction, normal) > 0 ? mediumInterface->outside : mediumInterface->inside;
	return medium;
}

const Medium* Interaction::GetMedium() const {
	return mediumInterface ? mediumInterface->inside : medium;
}

bool Interaction::IsSurfaceInteraction() const {
	return normal != Vec3(0.0f);
}

bool Interaction::IsMediumInteraction() const {
	return !IsSurfaceInteraction();
}

SurfaceInteraction::SurfaceInteraction(Vec3 position, const MediumInterface* mediumInterface)
	: Interaction(position, mediumInterface) {}

SurfaceInteraction::SurfaceInteraction(Vec3 position, Vec3 normal, Vec2 uv)
	: Interaction(position, normal, uv) {}

Spectrum SurfaceInteraction::Le(const glm::vec3& wo) const {
	return areaLight ? areaLight->L(position, normal, uv, wo) : Spectrum();
}

BSDF SurfaceInteraction::GetBSDF(const Ray& ray, const Camera* camera, Sampler* sampler) {
	return material->GetBSDF(*this);
}

Ray SurfaceInteraction::SpawnRay(const Ray& rayi, const BSDF& bsdf, Vec3 wi, BxDFFlags flags, Float eta) const {
	return Ray(position, wi);
}

void SurfaceInteraction::SkipIntersection(Ray& ray) const {
	ray = Ray(position + ray.direction * ShadowEpsilon, ray.direction);
}
