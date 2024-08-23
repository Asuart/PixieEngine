#include "pch.h"
#include "DiffuseAreaLight.h"

DiffuseAreaLight::DiffuseAreaLight(Shape* shape, const Material* material, const MediumInterface* mediumInterface)
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
    Vec3 offset = ss->intr.position - context.position;
    if (!ss || ss->pdf == 0 || length2(offset) == 0) {
        return {};
    }
    ss->intr.mediumInterface = m_mediumInterface;

    Vec3 wi = glm::normalize(offset);
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
