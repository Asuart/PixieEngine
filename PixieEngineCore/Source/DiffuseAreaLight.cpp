#include "pch.h"
#include "DiffuseAreaLight.h"

DiffuseAreaLight::DiffuseAreaLight(const Shape* shape, const Material* material, const MediumInterface* mediumInterface)
	: Light(LightType::Area, shape->GetTransform(), mediumInterface), m_shape(shape), m_material(material) {}

Spectrum DiffuseAreaLight::Phi() const {
	return Pi * 2.0f * m_shape->Area() * m_material->GetEmission();
}

Spectrum DiffuseAreaLight::L(Vec3 p, Vec3 n, Vec2 uv, Vec3 w) const {
	return m_material->GetEmission();
}

std::optional<LightLiSample> DiffuseAreaLight::SampleLi(LightSampleContext context, Vec2 u, bool allowIncompletePDF) const {
	ShapeSampleContext shapeContext = ShapeSampleContext(context.position, context.normal);
	std::optional<ShapeSample> ss = m_shape->Sample(shapeContext, u);
	if (!ss || ss->pdf == 0 || length2(ss->intr.position - context.position) == 0) {
		return {};
	}
	ss->intr.mediumInterface = m_mediumInterface;

	Vec3 wi = glm::normalize(ss->intr.position - context.position);
	Spectrum Le = L(ss->intr.position, ss->intr.normal, ss->intr.uv, -wi);
	if (!Le) {
		return {};
	}
	return LightLiSample(Le, wi, ss->pdf, ss->intr);
}

Float DiffuseAreaLight::SampleLiPDF(LightSampleContext context, Vec3 wi, bool allowIncompletePDF) const {
	ShapeSampleContext shapeCtx(context.position, context.normal);
	return m_shape->SamplePDF(shapeCtx, wi);
}

std::optional<LightLeSample> DiffuseAreaLight::SampleLe(Vec2 u1, Vec2 u2) const {
	std::optional<ShapeSample> ss = m_shape->Sample(u1);
	if (!ss) {
		return {};
	}
	ss->intr.mediumInterface = m_mediumInterface;

	Vec3 w;
	Float pdfDir;
	if (u2[0] < 0.5f) {
		u2[0] = std::min(u2[0] * 2, OneMinusEpsilon);
		w = SampleCosineHemisphere(u2);
	}
	else {
		u2[0] = std::min((u2[0] - 0.5f) * 2, OneMinusEpsilon);
		w = SampleCosineHemisphere(u2);
		w.z *= -1;
	}
	pdfDir = CosineHemispherePDF(std::abs(w.z)) / 2;
	if (pdfDir == 0) {
		return {};
	}

	const SurfaceInteraction& intr = ss->intr;
	Frame nFrame = Frame::FromZ(intr.normal);
	w = nFrame.FromLocal(w);
	Spectrum Le = L(intr.position, intr.normal, intr.uv, w);
	return LightLeSample(Le, Ray(intr.position, w), intr, ss->pdf, pdfDir);
}

void DiffuseAreaLight::SampleLePDF(const Ray& ray, Float* pdfPos, Float* pdfDir) const {
	std::cout << "ERROR: shouldn't be called for area lights.\n";
	exit(1);
}

void DiffuseAreaLight::SampleLePDF(const SurfaceInteraction& intr, Vec3 w, Float* pdfPos, Float* pdfDir) const {
	*pdfPos = m_shape->SamplePDF(intr);
	*pdfDir = CosineHemispherePDF(AbsDot(intr.normal, w)) / 2.0f;
}

Bounds3f DiffuseAreaLight::Bounds() const {
	return m_shape->Bounds();
}