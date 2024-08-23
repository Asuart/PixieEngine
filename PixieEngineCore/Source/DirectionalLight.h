#pragma once
#include "pch.h"
#include "Light.h"

class DirectionalLight : public Light {
public:
	DirectionalLight(Spectrum spectrum, float scale, const Transform& transform);
	
	Spectrum Phi() const override;
	std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	void Preprocess(const Bounds3f& sceneBounds) override;

protected:
	Spectrum m_spectrum;
	float m_scale;
	Vec3 m_sceneCenter = Vec3(0);
	Float m_sceneRadius = 0.0f;
};