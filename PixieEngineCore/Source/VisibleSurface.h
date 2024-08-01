#pragma once
#include "RTInteraction.h"

class VisibleSurface {
public:
	Vec3 p;
	Vec3 n;
	Vec2 uv;
	Vec3 albedo;
	bool set = false;

	VisibleSurface(const RTInteraction& si, Vec3 albedo);

	operator bool() const;
};
