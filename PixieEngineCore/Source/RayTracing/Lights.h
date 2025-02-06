#pragma once
#include "pch.h"
#include "Spectrum.h"
#include "Ray.h"
#include "RayInteraction.h"
#include "Shapes.h"
#include "Math/Bounds.h"
#include "Math/Transform.h"
#include "Resources/ResourceManager.h"

enum class LightType : uint32_t {
	DeltaPosition,
	DeltaDirection,
	Area,
	Infinite
};

// Checks if a light is defined using a Dirac delta distribution.
bool IsDeltaLight(LightType type);

struct LightSampleContext {
	Vec3 position = Vec3(0.0f);
	Vec3 normal = Vec3(0.0f);

	LightSampleContext() = default;
	LightSampleContext(const Vec3& position, const Vec3& normal = Vec3(0));
	LightSampleContext(const RayInteraction& intr);
};

struct LightLiSample {
	Spectrum L; // Amount of radiance.
	Vec3 wi; // Direction from light.
	Float pdf; // Probability to sample light point relatively to previously specified point.
	RayInteraction pLight; // Point light.

	LightLiSample(Spectrum L, Vec3 wi, Float pdf, const RayInteraction& pLight);
};

struct LightLeSample {
	Spectrum spectrum; // Radiance coming along the ray.
	Ray ray; // Random ray directed from light.
	Float pPosition; // Prpbability of sampling position.
	Float pDirection; // Probability of sampling direction.
	std::optional<RayInteraction> intr;

	LightLeSample(Spectrum spectrum, Ray ray, Float pPosition, Float pDirection);
	LightLeSample(Spectrum spectrum, Ray ray, RayInteraction intr, Float pPosition, Float pDirection);
};

class Light {
public:
	Light(LightType type, const Transform& transform);

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
	virtual void SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const = 0;
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
};

class PointLight : public Light {
public:
	PointLight(Spectrum spectrum, Float scale, const Transform& transform);

	Spectrum Phi() const;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const override;
	void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;
	void SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const override;
	Bounds3f Bounds() const;

protected:
	Spectrum m_spectrum;
	Float m_scale;
};

class SpotLight : public Light {
public:
	SpotLight(Spectrum spectrum, Float scale, Float totalWidth, Float falloffStart, const Transform& transform);

	Spectrum Phi() const;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;

protected:
	Spectrum m_spectrum;
	Float m_scale;
	Float m_cosFalloffStart;
	Float m_cosFalloffEnd;

	Spectrum I(Vec3 direction) const;
};

class DirectionalLight : public Light {
public:
	DirectionalLight(Spectrum spectrum, Float scale, const Transform& transform);

	Spectrum Phi() const override;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const override;
	void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;
	void SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const override;
	Bounds3f Bounds() const;
	void Preprocess(const Bounds3f& sceneBounds) override;

protected:
	Spectrum m_spectrum;
	Float m_scale;
	Vec3 m_sceneCenter = Vec3(0);
	Float m_sceneRadius = 100.0f;
};

class DiffuseAreaLight : public Light {
public:
	DiffuseAreaLight(const Shape* shape, Transform transform, int32_t materialIndex);

	Spectrum Phi() const override;
	Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const override;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;
	Spectrum Le(const Ray& ray) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const override;
	void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;
	void SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const override;
	Bounds3f Bounds() const override;

protected:
	const Shape* m_shape;
	const int32_t m_materialIndex;
};

class UniformInfiniteLight : public Light {
public:
	UniformInfiniteLight(Spectrum spectrum, Float scale, const Transform& transform);

	Spectrum Phi() const override;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 w, bool allowIncompletePDF = false) const override;
	Spectrum Le(const Ray& ray) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const;
	void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;
	void SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const override;
	Bounds3f Bounds() const;
	void Preprocess(const Bounds3f& sceneBounds) override;

protected:
	Spectrum m_spectrum;
	Float m_scale;
	Vec3 m_sceneCenter = Vec3(0);
	Float m_sceneRadius = 0.0f;
};

class GradientInfiniteLight : public Light {
public:
	GradientInfiniteLight(Spectrum topSpectrum, Spectrum bottomSpectrum, Float scale, const Transform& transform);

	Spectrum Phi() const override;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 w, bool allowIncompletePDF = false) const override;
	Spectrum Le(const Ray& ray) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const;
	void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;
	void SampleLePDF(const RayInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const override;
	Bounds3f Bounds() const override;
	void Preprocess(const Bounds3f& sceneBounds) override;

protected:
	Spectrum m_topSpectrum;
	Spectrum m_bottomSpectrum;
	Float m_scale;
	Vec3 m_sceneCenter = Vec3(0);
	Float m_sceneRadius = 0.0f;

	Spectrum GetSpectrum(Vec3 direction) const;
};
