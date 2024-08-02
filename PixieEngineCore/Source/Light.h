#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTInteraction.h"
#include "Shape.h"

struct LightLiSample {
	glm::fvec3 L = glm::fvec3(0.0f);
	Vec3 wi = Vec3(0.0f);
	Float pdf = 0;
	RTInteraction pLight;

	LightLiSample(const glm::fvec3& L, Vec3 wi, Float pdf, const RTInteraction& pLight);
};

struct LightSampleContext {
	Vec3 p;
	Vec3 n;
};

class DiffuseAreaLight {
public:
	Shape* shape = nullptr;
	bool twoSided = true;
	glm::fvec3 Lemit = glm::fvec3(1.0);
	Float scale = 1.0;

	DiffuseAreaLight(Shape* shape, glm::fvec3 emit = glm::fvec3(1), Float scale = 1);

	glm::fvec3 L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const;
	std::optional<LightLiSample> SampleLi(RTInteraction intr, Vec2 u) const;
};

struct SampledLight {
	DiffuseAreaLight light;
	Float p = 0;
};

class UniformLightSampler {
	std::vector<DiffuseAreaLight> lights;
public:

	UniformLightSampler(const std::vector<DiffuseAreaLight>& _lights);

	std::optional<SampledLight> Sample(Float u) const;
	Float PMF() const;
};
