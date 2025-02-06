#pragma once
#include "pch.h"
#include "Spectrum.h"
#include "Samplers.h"
#include "LightSamplers.h"
#include "GBufferPixel.h"
#include "MaterialSample.h"

class SceneSnapshot;

constexpr int32_t MaxRayBounces = 1024;

enum class RayTracingVisualization : uint32_t {
	LightAccumulation = 0,
	BoxTest,
	ShapeTest,
	Depth,
	Normals,
	COUNT
};

enum class RayTracingMode : uint32_t {
	Naive,
	Volumetric,
	COUNT
};

std::string to_string(RayTracingVisualization mode);
std::string to_string(RayTracingMode mode);

class RayTracer {
public:
	virtual void SetSceneSnapshot(SceneSnapshot* sceneSnapshot);
	virtual GBufferPixel SampleLightRay(Ray ray, Sampler* sampler) = 0;

	bool Unoccluded(const RayInteraction& p0, const RayInteraction& p1, GBufferPixel& pixel);
	bool Unoccluded(Vec3 p0, Vec3 p1, GBufferPixel& pixel);

protected:
	SceneSnapshot* m_sceneSnapshot = nullptr;
};

class NaiveRayTracer : public RayTracer {
public:
	bool m_regularize = true;

	NaiveRayTracer();
	~NaiveRayTracer();

	void SetSceneSnapshot(SceneSnapshot* scene) override;
	GBufferPixel SampleLightRay(Ray ray, Sampler* sampler) override;

protected:
	LightSampler* m_lightSampler = nullptr;

	MaterialSample SampleMaterial(const Material& material, RayInteraction& inter, Vec3 wo, Vec3 wi);
	MaterialSample SampleMaterial(const Material& material, RayInteraction& inter, Sampler* sampler);
	Spectrum SampleLd(RayInteraction& intr, const Material& material, Sampler* sampler, GBufferPixel& pixel);
};

class VolumetricRayTracer : public RayTracer {
public:
	void SetSceneSnapshot(SceneSnapshot* sceneSnapshot) override;
	GBufferPixel SampleLightRay(Ray ray, Sampler* sampler) override;

protected:
	LightSampler* m_lightSampler;
	bool m_regularize = true;

	Spectrum SampleLd(const RayInteraction& intr, const BSDF* bsdf, Sampler* sampler, Spectrum beta, Spectrum r_p, GBufferPixel& pixel);
};

RayTracer* CreateRayTracer(RayTracingMode mode);
