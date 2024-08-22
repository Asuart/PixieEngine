#pragma once
#include "pch.h"
#include "Light.h"

class PointLight : public Light {
public:
	PointLight(Spectrum spectrum, float scale, const Transform& transform, MediumInterface* mediumInterface);

	Spectrum Phi() const;
    std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;

protected:
	Spectrum m_spectrum;
	float m_scale;
};