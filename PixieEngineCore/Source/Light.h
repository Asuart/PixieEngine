#pragma once
#include "pch.h"
#include "Spectrum.h"
#include "Ray.h"
#include "Interaction.h"
#include "Triangle.h"
#include "Bounds.h"
#include "Transform.h"

enum class LightType : uint32_t {
	DeltaPosition,
	DeltaDirection,
	Area,
	Infinite
};

// Checks if a light is defined using a Dirac delta distribution.
bool IsDeltaLight(LightType type);

struct LightSampleContext {
	Vec3 position;
	Vec3 normal;

	LightSampleContext(const Vec3& position, const Vec3& normal = Vec3(0));
	LightSampleContext(const SurfaceInteraction& intr);
};

struct LightLiSample {
	Spectrum L; // Amount of radiance.
	Vec3 wi; // Direction from light.
	Float pdf; // Probability to sample light point relatively to previously specified point.
	SurfaceInteraction pLight; // Point light.

	LightLiSample(Spectrum L, Vec3 wi, Float pdf, const SurfaceInteraction& pLight);
};

struct LightLeSample {
	Spectrum spectrum; // Radiance coming along the ray.
	Ray ray; // Random ray directed from light.
	Float pPosition; // Prpbability of sampling position.
	Float pDirection; // Probability of sampling direction.
	std::optional<SurfaceInteraction> intr;

	LightLeSample(Spectrum spectrum, Ray ray, Float pPosition, Float pDirection);
	LightLeSample(Spectrum spectrum, Ray ray, SurfaceInteraction intr, Float pPosition, Float pDirection);
};

class Light {
public:
	Light(LightType type, const Transform& transform, const MediumInterface* mediumInterface);

	// Amount of light emitted by light source.
	virtual Spectrum Phi() const = 0;
	// Returns incident radiance from the light at a point and direction from which it is arriving.
	virtual std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const = 0;
	// Probabilty of light arriving at a point from specified direction.
	virtual Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const = 0;
	// Sample spectrum and random ray coming from light source.
	virtual std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const = 0;
	// Sample probabilities of light coming from light along specified ray.
	virtual void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const = 0;
	virtual void SampleLePDF(const SurfaceInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const = 0;
	// Sample light that has geometry.
	virtual Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const;
	// Sample infinite area light.
	virtual Spectrum Le(const Ray& ray) const;
	virtual Bounds3f Bounds() const = 0;
	virtual LightType Type() const;
	virtual void Preprocess(const Bounds3f& sceneBounds);

	const Transform& GetTransform();

protected:
	LightType m_type;
	Transform m_transform;
	const MediumInterface* m_mediumInterface;
};
