#pragma once
#include "pch.h"
#include "Interaction.h"

struct ShapeSampleContext {
	Vec3 position;
	Vec3 normal;

	ShapeSampleContext(const Vec3& position, const Vec3& normal = Vec3(0));
	ShapeSampleContext(const SurfaceInteraction& intr);
};