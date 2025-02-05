#pragma once
#include "BSDF.h"

struct MaterialSample {
	Spectrum f = Spectrum(0.0f);
	Vec3 scattered = Vec3(0.0f);
	Float pdf = 0.0f;
	BxDFFlags flags = BxDFFlags::Unset;
	Float transmission = 0;
	Float refraction = 0;

	MaterialSample() = default;
	MaterialSample(Spectrum _f, Vec3 _scattered, Float _pdf, BxDFFlags _flags = BxDFFlags::Unset) : 
		f(_f), scattered(_scattered), pdf(_pdf), flags(_flags) {}
};
