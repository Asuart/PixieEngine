#pragma once
#include "Interaction.h"

class VisibleSurface {
public:
	Vec3 p;
	Vec3 n;
	Vec2 uv;
	Vec3 albedo;
	bool set = false;

	VisibleSurface(const SurfaceInteraction& si, Vec3 albedo);

	operator bool() const;
};
