#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Interaction.h"
#include "Shape.h"

struct LightLiSample {
	glm::fvec3 L = glm::fvec3(0.0f);
	Vec3 wi = Vec3(0.0f);
	Float pdf = 0;
	SurfaceInteraction pLight;

	LightLiSample(const glm::fvec3& L, Vec3 wi, Float pdf, const SurfaceInteraction& pLight);
};

struct LightSampleContext {
	Vec3 p;
	Vec3 n;
};

class AreaLight {
public:
	Shape* shape = nullptr;
	glm::fvec3 Lemit = glm::fvec3(1.0);
	Float scale = 1.0;
	bool twoSided = true;

	virtual glm::fvec3 L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const = 0;
	virtual std::optional<LightLiSample> SampleLi(SurfaceInteraction intr, Vec2 u) const = 0;

protected:
	AreaLight(Shape* shape, glm::fvec3 emit = glm::fvec3(1), Float scale = 1);
};

class DiffuseAreaLight : public AreaLight {
public:
	DiffuseAreaLight(Shape* shape, glm::fvec3 emit = glm::fvec3(1), Float scale = 1);

	virtual glm::fvec3 L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const;
	virtual std::optional<LightLiSample> SampleLi(SurfaceInteraction intr, Vec2 u) const;
};

struct SampledLight {
	AreaLight* light;
	Float p = 0;
};

class UniformLightSampler {
	std::vector<AreaLight*> lights;
public:

	UniformLightSampler(const std::vector<AreaLight*>& _lights);

	std::optional<SampledLight> Sample(Float u) const;
	Float PMF() const;
};
