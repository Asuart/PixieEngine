#pragma once
#include "pch.h"
#include "Spectrum.h"
#include "Samplers.h"
#include "LightSamplers.h"
#include "GBufferPixel.h"
#include "MaterialSample.h"

class SceneSnapshot;

enum class RayTracingVisualization : uint32_t {
	LightAccumulation = 0,
	BoxTest,
	ShapeTest,
	Depth,
	Normals,
	COUNT
};

std::string to_string(RayTracingVisualization mode);

class VolumetricRayTracer {
public:
	void SetSceneSnapshot(SceneSnapshot* sceneSnapshot);
	GBufferPixel SampleLightRay(Ray ray, Sampler* sampler);

protected:
	SceneSnapshot* m_sceneSnapshot = nullptr;
	LightSampler* m_lightSampler = nullptr;
	bool m_regularize = true;

	bool Unoccluded(const RayInteraction& p0, const RayInteraction& p1, GBufferPixel& pixel);
	bool Unoccluded(Vec3 p0, Vec3 p1, GBufferPixel& pixel);
	Spectrum SampleLd(const RayInteraction& intr, const BSDF* bsdf, Sampler* sampler, Spectrum beta, Spectrum r_p, GBufferPixel& pixel);
};
