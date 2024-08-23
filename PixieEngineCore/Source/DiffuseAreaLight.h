#pragma once
#include "pch.h"
#include "Light.h"
#include "Shape.h"

class DiffuseAreaLight : public Light {
public:
	DiffuseAreaLight(Shape* shape, const Material* material, const MediumInterface* mediumInterface);

	virtual Spectrum Phi() const override;
	virtual Spectrum L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const override;
	virtual std::optional<LightLiSample> SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF = false) const override;
	virtual Float SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF = false) const override;

protected:
	Shape* m_shape;
	const Material* m_material = nullptr;
};