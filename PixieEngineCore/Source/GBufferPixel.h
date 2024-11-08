#pragma once
#include "Interaction.h"

class GBufferPixel {
public:
	Vec3 p;
	Vec3 n;
	Vec2 uv;
	Spectrum albedo;
	Float depth;
	bool set = false;

	GBufferPixel(const SurfaceInteraction& si, Spectrum albedo, Float depth);

	operator bool() const;
};
