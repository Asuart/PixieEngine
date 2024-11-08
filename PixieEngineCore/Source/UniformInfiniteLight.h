#pragma once
#include "pch.h"
#include "Light.h"

class UniformInfiniteLight : public Light {
public:
	UniformInfiniteLight(Spectrum spectrum, Float scale, const Transform& transform);

    Spectrum Phi() const override;
    std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
    Float SampleLiPDF(LightSampleContext context, Vec3 w, bool allowIncompletePDF = false) const override;
	Spectrum Le(const Ray& ray) const override;
	std::optional<LightLeSample> SampleLe(Vec2 u1, Vec2 u2) const;
    void SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const;
	void Preprocess(const Bounds3f& sceneBounds) override;

protected:
	Spectrum m_spectrum;
	Float m_scale;
	Vec3 m_sceneCenter = Vec3(0);
	Float m_sceneRadius = 0.0f;
};
