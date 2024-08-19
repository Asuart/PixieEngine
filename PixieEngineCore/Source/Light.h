#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Interaction.h"
#include "Shape.h"
#include "Material.h"

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

struct Light {
	Spectrum m_emission;

	Light(Spectrum emission = Spectrum());

	virtual Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const = 0;
	virtual Spectrum Le(const Ray& ray);
	virtual Float PDF_Li(SurfaceInteraction ctx, Vec3 wi, bool allowIncompletePDF = false) const = 0;
};

class AreaLight : public Light {
public:
	Shape* shape = nullptr;
	const Material* material = nullptr;
	bool twoSided = true;

	virtual std::optional<LightLiSample> SampleLi(SurfaceInteraction intr, Vec2 u) const = 0;

protected:
	AreaLight(Shape* shape, const Material* material);
};

class DiffuseAreaLight : public AreaLight {
public:
	DiffuseAreaLight(Shape* shape, const Material* material);

	virtual Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const;
	virtual std::optional<LightLiSample> SampleLi(SurfaceInteraction intr, Vec2 u) const;
	virtual Float PDF_Li(SurfaceInteraction ctx, Vec3 wi, bool allowIncompletePDF = false) const;
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
	Float PMF(const Light* light) const;
	Float PMF(const SurfaceInteraction& ctx, const Light* light) const;
};
