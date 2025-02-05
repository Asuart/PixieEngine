#include "pch.h"
#include "Material.h"

Vec3 Refract(const Vec3& uv, const Vec3& n, Float etai_over_etat) {
	Float cos_theta = fmin(glm::dot(-uv, n), 1.0f);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = (Float)(-sqrt(fabs(1.0f - length2(r_out_perp)))) * n;
	return r_out_perp + r_out_parallel;
}

Float Reflectance(Float cosine, Float ref_idx) {
	Float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
}

Material::Material(const std::string& name, Spectrum albedo, Spectrum emissionColor, float emissionStrength, float roughness, float metallic, float transparency, float refraction) :
	m_name(name), m_emissionColor(emissionColor), m_emissionStrength(emissionStrength), m_albedo(albedo), m_roughness(roughness), m_metallic(metallic), m_transparency(transparency), m_refraction(refraction) {
	assert(!isnan(m_emissionColor.GetRGB().x) && !isnan(m_emissionColor.GetRGB().y) && !isnan(m_emissionColor.GetRGB().z));
	assert(!isnan(m_emissionStrength) && !isnan(m_albedo.GetRGB().x) && !isnan(m_albedo.GetRGB().y) && !isnan(m_albedo.GetRGB().z));
	assert(!isnan(m_roughness) && !isnan(m_metallic) && !isnan(m_transparency) && !isnan(m_refraction));
}

bool Material::IsEmissive() {
	return m_emissionStrength > 0.0f;
}

bool Material::IsTranslucent() {
	return m_transparency > 0.0f;
}

Spectrum Material::GetEmission() {
	return m_emissionColor * m_emissionStrength;
}

Spectrum Material::Evaluate(const RayInteraction& intr) {
	Vec3 color = m_albedo.GetRGB();
	color *= m_albedoTexture.Sample(intr.uv);
	return color / Pi;
}

Float Material::Pdf() {
	return Inv2Pi;
}

BSDF Material::GetBSDF(const RayInteraction& intr) {
	return BSDF(*this, intr);
}

BxDFFlags Material::Flags() {
	return BxDFFlags::Unset;
}
