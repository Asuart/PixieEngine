#include "pch.h"
#include "Interaction.h"
#include "Light.h"
#include "Material.h"
#include "Ray.h"
#include "Camera.h"
#include "TriangleCache.h"

Vec3 Interaction::OffsetRayOrigin(Vec3 origin) const {
    return origin + normal * ShadowEpsilon;
}

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

Spectrum SurfaceInteraction::Le(const glm::vec3& wo) const {
    return areaLight ? areaLight->L(position, normal, uv, wo) : Spectrum();
}

BSDF SurfaceInteraction::GetBSDF(const Ray& ray, const Camera* camera, Sampler* sampler) {
    return material->GetBSDF(*this);
}

Ray SurfaceInteraction::SpawnRay(const Ray& rayi, const BSDF& bsdf, Vec3 wi, int32_t flags, Float eta) const {
    Ray rd = Ray(rayi.x, rayi.y, position, wi);
    return rd;
}

void SurfaceInteraction::SkipIntersection(Ray& ray) const {
    ray = Ray(ray.x, ray.y, position + ray.direction * ShadowEpsilon, ray.direction);
}