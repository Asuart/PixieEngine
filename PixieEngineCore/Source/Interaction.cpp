#include "Interaction.h"

Vec3 Interaction::OffsetRayOrigin(Vec3 origin) const {
    return origin + normal * ShadowEpsilon;
}

Ray Interaction::SpawnRayTo(Vec3 point) const {
    Ray ray = Ray(position, point - position);
    ray.medium = GetMedium(ray.direction);
    return ray;
}

Ray Interaction::SpawnRayTo(const Interaction& it) const {
    Ray ray = Ray(position, it.position - position);
    ray.medium = GetMedium(ray.direction);
    return ray;
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
