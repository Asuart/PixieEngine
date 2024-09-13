#pragma once
#include "pch.h"
#include "Light.h"
#include "Shape.h"
#include "Material.h"

class DiffuseAreaLight : public Light {
public:
	DiffuseAreaLight(const Shape* shape, const Material* material, const MediumInterface* mediumInterface = nullptr);

	Spectrum Phi() const override;
	Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const override;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const override;
	void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;
	void SampleLePDF(const SurfaceInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const override;
	Bounds3f Bounds() const override;

protected:
	const Shape* m_shape;
	const Material* m_material;
};
