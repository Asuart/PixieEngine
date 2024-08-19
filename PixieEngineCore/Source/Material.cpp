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

Spectrum Material::Evaluate(const SurfaceInteraction& collision) const {
	glm::fvec3 color = m_albedo.GetRGBValue();
	if (m_albedoTexture) {
		color *= m_albedoTexture->Sample(collision);
	}
	return color / Pi;
}

float Material::Pdf() const {
	return Inv2Pi;
}

Spectrum Material::Sample(const Ray& ray, const SurfaceInteraction& collision, Ray& scatteredRay) const {
	Vec3 scatteredDirection;

	if (m_transparency > RandomFloat()) {
		Float refraction = !collision.backface ? m_refraction : (1.0f / m_refraction);

		Float cosTheta = glm::dot(-ray.direction, collision.normal);
		Float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

		bool cannotRefract = refraction * sinTheta > 1.0f;
		if (cannotRefract || Reflectance(cosTheta, refraction) > RandomFloat()) {
			scatteredDirection = glm::reflect(ray.direction, collision.normal);
		}
		else {
			scatteredDirection = glm::refract(ray.direction, collision.normal, refraction);
		}
	}
	else {
		Frame shadingFrame = Frame::FromZ(collision.normal);
		scatteredDirection = shadingFrame.FromLocal(SampleUniformHemisphere(Vec2(RandomFloat(), RandomFloat())));
	}

	scatteredRay.origin = collision.position;
	scatteredRay.direction = scatteredDirection;
	Float cosTheta = glm::abs(glm::dot(collision.normal, scatteredRay.direction));
	return (cosTheta * Evaluate(collision)) / Pdf();
}

BxDF* Material::GetBxDF(const SurfaceInteraction& intr) const {
	if (RandomFloat() > m_transparency) {
		return new DiffuseBxDF(Evaluate(intr));
	}
	else {
		const glm::fvec2 anisotropicRoughness = glm::fvec2(1.0f, 1.0f);
		const bool normalizeAnisotropicRoughness = false;

		float urough = m_roughness * anisotropicRoughness.x;
		float vrough = m_roughness * anisotropicRoughness.y;
		if (normalizeAnisotropicRoughness) {
			urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
			vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
		}

		TrowbridgeReitzDistribution distrib(urough, vrough);
		return new DielectricBxDF(m_refraction, distrib);
	}
	//return new DiffuseTransmissionBxDF(reflectance * scale, transmittance * scale);
}

BSDF Material::GetBSDF(const SurfaceInteraction& intr) const {
	BxDF* bxdf = GetBxDF(intr);
	return BSDF(intr.normal, intr.dpdu, bxdf);
}
