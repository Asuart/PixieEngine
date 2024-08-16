#include "Material.h"

Material::Material(const std::string& name, Vec3 _albedo, Vec3 _emission, Float _roughness, Float _metallic, Float _transparency, Float _eta)
	: name(name), albedo(_albedo), emission(_emission), roughness(_roughness), metallic(_metallic), transparency(_transparency), eta(_eta),
	m_emission(_emission), m_albedo(_albedo), m_roughness(_roughness), m_metallic(_metallic), m_transparency(_transparency), m_refraction(_eta) {}

bool Material::IsEmissive() {
	return m_emission.GetRGBValue() != glm::fvec3(0.0f, 0.0f, 0.0f);
}

bool Material::IsTranslucent() {
	return m_transparency > 0.0f;
}

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

inline Vec3 Material::Evaluate(const SurfaceInteraction& collision) const {
	Vec3 color = albedo;
	if (rtTexture) {
		color *= rtTexture->Sample(collision);
	}
	return color / Pi;
}

inline constexpr Float Material::Pdf() const {
	return Inv2Pi;
}

Vec3 Material::Sample(const Ray& ray, const SurfaceInteraction& collision, Ray& scatteredRay) const {
	Vec3 scatteredDirection;

	if (transparency > RandomFloat()) {
		Float refraction = !collision.backface ? eta : (1.0f / eta);

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
	return new DiffuseBxDF(rtTexture->Sample(intr));
}

BSDF Material::GetBSDF(const SurfaceInteraction& intr) const {
	BxDF* bxdf = GetBxDF(intr);
	return BSDF(intr.normal, intr.dpdu, bxdf);
}


//BxDF* DiffuseMaterial::GetBxDF(const SurfaceInteraction& intr) const {
//	return new DiffuseBxDF(rtTexture->Sample(intr));
//}

//BxDF* DiffuseTransmissionMaterial::GetBxDF(const SurfaceInteraction& intr) const {
//	return new DiffuseTransmissionBxDF(reflectance * scale, transmittance * scale);
//}

//BxDF* DielectricMaterial::GetBxDF(const SurfaceInteraction& intr) const {
//	Float urough = uRoughness, vrough = vRoughness;
//	if (false) {
//		urough = TrowbridgeReitzDistribution::RoughnessToAlpha(urough);
//		vrough = TrowbridgeReitzDistribution::RoughnessToAlpha(vrough);
//	}
//	TrowbridgeReitzDistribution distrib(urough, vrough);
//	return new DielectricBxDF(eta, distrib);
//}