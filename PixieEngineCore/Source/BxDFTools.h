#pragma once
#include "pch.h"

enum class TransportMode {
	Radiance,
	Importance
};

enum class LightFlags : uint32_t {
	DeltaPosition = 1,
	DeltaDirection = 2,
	Area = 4,
	Infinite = 8
};

enum BxDFFlags {
	Unset = 0,
	Reflection = 1 << 0,
	Transmission = 1 << 1,
	Diffuse = 1 << 2,
	Glossy = 1 << 3,
	Specular = 1 << 4,
	DiffuseReflection = Diffuse | Reflection,
	DiffuseTransmission = Diffuse | Transmission,
	GlossyReflection = Glossy | Reflection,
	GlossyTransmission = Glossy | Transmission,
	SpecularReflection = Specular | Reflection,
	SpecularTransmission = Specular | Transmission,
	All = Diffuse | Glossy | Specular | Reflection | Transmission
};

enum class BxDFReflTransFlags {
	Unset = 0,
	Reflection = 1 << 0,
	Transmission = 1 << 1,
	All = Reflection | Transmission
};

bool IsReflective(BxDFFlags f);
bool IsTransmissive(BxDFFlags f);
bool IsDiffuse(BxDFFlags f);
bool IsGlossy(BxDFFlags f);
bool IsSpecular(BxDFFlags f);
bool IsNonSpecular(BxDFFlags f);