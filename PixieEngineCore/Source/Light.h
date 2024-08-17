#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Interaction.h"
#include "Shape.h"

struct LightLiSample {
	Spectrum L = Spectrum();
	Vec3 wi = Vec3(0.0f);
	Float pdf = 0;
	SurfaceInteraction pLight;

	LightLiSample(Spectrum L, Vec3 wi, Float pdf, const SurfaceInteraction& pLight);
};

struct LightSampleContext {
	Vec3 p;
	Vec3 n;
};

class AreaLight {
public:
	Shape* shape = nullptr;
	Spectrum Lemit = Spectrum(1.0f, 1.0f, 1.0f);
	float scale = 1.0f;
	bool twoSided = true;

	virtual Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const = 0;
	virtual std::optional<LightLiSample> SampleLi(SurfaceInteraction intr, Vec2 u) const = 0;

protected:
	AreaLight(Shape* shape, Spectrum emit = Spectrum(1.0f, 1.0f, 1.0f), float scale = 1.0f);
};

class DiffuseAreaLight : public AreaLight {
public:
	DiffuseAreaLight(Shape* shape, Spectrum emit = Spectrum(1.0f, 1.0f, 1.0f), Float scale = 1);

	virtual Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const;
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
