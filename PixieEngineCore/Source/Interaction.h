#pragma once
#include "pch.h"

struct RayInteraction {
	Vec3 position = Vec3(0.0f);
	Vec3 normal = Vec3(0.0f);
	Vec2 uv = Vec2(0.0f);
	Vec3 wo = Vec3(0.0f);
	int32_t materialIndex = 0;
	int32_t lightIndex = -1;

	RayInteraction() = default;
	explicit RayInteraction(Vec3 position);
	RayInteraction(Vec3 position, Vec3 normal, Vec2 uv, Vec3 wo = Vec3(0));

	bool IsSurfaceInteraction() const;
	bool IsMediumInteraction() const;
};
