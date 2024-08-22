#pragma once
#include "pch.h"
#include "Interaction.h"
#include "TriangleCache.h"
#include "Material.h"
#include "Bounds.h"
#include "LightSampleContext.h"
#include "LightLiSample.h"
#include "Transform.h"

enum class LightType : uint32_t {
	DeltaPosition,
	DeltaDirection,
	Area,
	Infinite
};

// Checks if a light is defined using a Dirac delta distribution.
bool IsDeltaLight(LightType type);

class Light {
public:
	Light(LightType type, Transform transform, MediumInterface* mediumInterface);

	// Amount of light emitted by light source.
	virtual Spectrum Phi() const = 0;
	// Returns incident radiance from the light at a point and direction from which it is arriving.
	virtual std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const = 0;
	// Probabilty of light arriving at a point from specified direction.
	virtual Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const = 0;
	// Sample light that has geometry.
	virtual Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const;
	// Sample infinite area light.
	virtual Spectrum Le(const Ray& ray) const;
	virtual Bounds3f Bounds() const = 0;
	virtual LightType Type() const;
	virtual void Preprocess(const Bounds3f& sceneBounds);

protected:
	LightType m_type;
	Transform m_transform;
	MediumInterface* m_mediumInterface;
};

class AreaLight : public Light {
public:
	TriangleCache triangle;
	const Material* material = nullptr;

	virtual std::optional<LightLiSample> SampleLi(SurfaceInteraction intr, Vec2 u) const = 0;

protected:
	AreaLight(TriangleCache triangle, const Material* material);
};

class DiffuseAreaLight : public AreaLight {
public:
	DiffuseAreaLight(TriangleCache triangle, const Material* material);

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
