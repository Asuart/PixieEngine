#include "pch.h"
#include "Interaction.h"
#include "Material.h"
#include "Camera.h"
#include "ResourceManager.h"

RayInteraction::RayInteraction(Vec3 position) :
	position(position) {}

RayInteraction::RayInteraction(Vec3 position, Vec3 normal, Vec2 uv, Vec3 wo) :
	position(position), normal(normal), uv(uv), wo(wo) {}


bool RayInteraction::IsSurfaceInteraction() const {
	return normal != Vec3(0.0f);
}

bool RayInteraction::IsMediumInteraction() const {
	return !IsSurfaceInteraction();
}
