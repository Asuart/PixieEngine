#include "pch.h"
#include "Material.h"

Vec3 Refract(const Vec3& uv, const Vec3& n, Float etai_over_etat) {
	Float cos_theta = fmin(glm::dot(-uv, n), 1.0f);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = (Float)(-sqrt(fabs(1.0f - length2(r_out_perp)))) * n;
	return r_out_perp + r_out_parallel;
}

float Reflectance(Float cosine, Float ref_idx) {
	Float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
}

Material::Material(const std::string& name, Spectrum albedo, Spectrum emissionColor, float emissionStrength, float roughness, float metallic, float transparency, float refraction)
	: m_name(name),	m_emissionColor(emissionColor), m_emissionStrength(emissionStrength), m_albedo(albedo), m_roughness(roughness), m_metallic(metallic), m_transparency(transparency), m_refraction(refraction) {}

bool Material::IsEmissive() {
	return m_emissionStrength > 0.0f;
}

bool Material::IsTranslucent() {
	return m_transparency > 0.0f;
}

Spectrum Material::GetEmission() const {
	return m_emissionColor * m_emissionStrength;
}

Spectrum Material::Evaluate(const SurfaceInteraction& intr) const {
	glm::fvec3 color = m_albedo.GetRGBValue();
	if (m_albedoTexture) {
		color *= m_albedoTexture->Sample(intr);
	}
	return color / Pi;
}

float Material::Pdf() const {
	return Inv2Pi;
}

BSDF Material::GetBSDF(const SurfaceInteraction& intr) const {
	return BSDF(*this, intr);
}
