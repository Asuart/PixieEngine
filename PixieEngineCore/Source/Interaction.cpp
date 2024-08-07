#include "Interaction.h"
#include "Light.h"

Vec3 Interaction::OffsetRayOrigin(Vec3 origin) const {
    return origin + normal * ShadowEpsilon;
}

Medium* Interaction::GetMedium(Vec3 direction) const {
    if (mediumInterface) return glm::dot(direction, normal) > 0 ? mediumInterface->outside : mediumInterface->inside;
    return medium;
}

Medium* Interaction::GetMedium() const {
    return mediumInterface ? mediumInterface->inside : medium;
}

bool Interaction::IsSurfaceInteraction() const { 
    return normal != Vec3(0.0f); 
}

bool Interaction::IsMediumInteraction() const { 
    return !IsSurfaceInteraction(); 
}

glm::fvec3 SurfaceInteraction::Le(const glm::vec3& wo) const {
    return areaLight ? areaLight->L(position, normal, uv, wo) : glm::fvec3(0.0f);
}