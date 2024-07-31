#pragma once
#include "pch.h"
#include "BxDFTools.h"

struct BSDFSample {
	glm::vec3 f;
	glm::vec3 wi;
	float pdf = 0;
	BxDFFlags flags;
	float eta = 1;
	bool pdfIsProportional = false;

	BSDFSample(glm::vec3 f, glm::vec3 wi, float pdf, BxDFFlags flags, float eta = 1, bool pdfIsProportional = false);

	bool IsReflection() const;
	bool IsTransmission() const;
	bool IsDiffuse() const;
	bool IsGlossy() const;
	bool IsSpecular() const;
};
