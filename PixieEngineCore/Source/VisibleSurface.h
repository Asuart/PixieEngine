#pragma once
#include "Interaction.h"

class VisibleSurface {
public:
	Vec3 p;
	Vec3 n;
	Vec2 uv;
	Spectrum albedo;
	bool set = false;

	VisibleSurface(const SurfaceInteraction& si, Spectrum albedo);

	operator bool() const;
};
