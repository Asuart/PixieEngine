#pragma once 
#include "pch.h"
#include "Light.h"

class SpotLight : public Light {
public:
	SpotLight(Spectrum spectrum, Float scale, Float totalWidth, Float falloffStart, const Transform& transform, MediumInterface* mediumInterface);

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