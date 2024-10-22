#pragma once
#include "pch.h"
#include "Light.h"

class PointLight : public Light {
public:
	PointLight(Spectrum spectrum, Float scale, const Transform& transform, MediumInterface* mediumInterface);

	Spectrum Phi() const;
    std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const override;
	void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const override;
	void SampleLePDF(const SurfaceInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const override;
	Bounds3f Bounds() const;

protected:
	Spectrum m_spectrum;
	Float m_scale;
};