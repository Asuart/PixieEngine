#pragma once
#include "pch.h"
#include "Interaction.h"

struct LightSampleContext {
	Vec3 position;
	Vec3 normal;

	LightSampleContext(const Vec3& position, const Vec3& normal = Vec3(0));
	LightSampleContext(const SurfaceInteraction& intr);
};