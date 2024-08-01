#pragma once
#include "PixieEngineCoreHeaders.h"
#include "RTInteraction.h"
#include "Shape.h"

struct LightLiSample {
	Vec3 L = Vec3(0);
	Vec3 wi = Vec3(0);
	Float pdf = 0;
	RTInteraction pLight;

	LightLiSample(const Vec3& L, Vec3 wi, Float pdf, const RTInteraction& pLight);
};

struct LightSampleContext {
	Vec3 p;
	Vec3 n;
};

class DiffuseAreaLight {
public:
	Shape* shape = nullptr;
	bool twoSided = true;
	Vec3 Lemit = Vec3(1.0);
	Float scale = 1.0;

	DiffuseAreaLight(Shape* shape, Vec3 emit = Vec3(1), Float scale = 1);

	Vec3 L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const;
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
