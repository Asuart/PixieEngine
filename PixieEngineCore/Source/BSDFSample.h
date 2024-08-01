#pragma once
#include "PixieEngineCoreHeaders.h"
#include "BxDFTools.h"

struct BSDFSample {
	Vec3 f;
	Vec3 wi;
	Float pdf = 0;
	BxDFFlags flags;
	Float eta = 1;
	bool pdfIsProportional = false;

	BSDFSample(Vec3 f, Vec3 wi, Float pdf, BxDFFlags flags, Float eta = 1, bool pdfIsProportional = false);

	bool IsReflection() const;
	bool IsTransmission() const;
	bool IsDiffuse() const;
	bool IsGlossy() const;
	bool IsSpecular() const;
};
