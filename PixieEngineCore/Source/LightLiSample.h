#pragma once
#include "pch.h"
#include "Spectrum.h"
#include "Interaction.h"

struct LightLiSample {
	Spectrum L; // Amount of radiance.
	Vec3 wi; // Direction from light.
	Float pdf; // Probability to sample light point relatively to previously specified point.
	SurfaceInteraction pLight; // Point light.

	LightLiSample(Spectrum L, Vec3 wi, Float pdf, const SurfaceInteraction& pLight);
};