#include "pch.h"
#include "RTMaterial.h"

glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat) {
	float cos_theta = fmin(glm::dot(-uv, n), 1.0f);
	glm::vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	glm::vec3 r_out_parallel = -sqrt(fabs(1.0f - glm::length2(r_out_perp))) * n;
	return r_out_perp + r_out_parallel;
}

float Reflectance(float cosine, float ref_idx) {
	auto r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0);
}

inline glm::vec3 RTMaterial::Evaluate(const RTInteraction& collision) const {
	glm::vec3 color = albedo;
	if (texture) {
		color *= texture->Sample(collision);
	}
	return color / Pi;
}

inline constexpr float RTMaterial::Pdf() const {
	return Inv2Pi;
}

glm::vec3 RTMaterial::Sample(const Ray& ray, const RTInteraction& collision, Ray& scatteredRay) const {
	glm::vec3 scatteredDirection;

	if (transparency > RandomFloat()) {
		float refraction = !collision.backFace ? eta : (1.0 / eta);

		float cosTheta = glm::dot(-ray.d, collision.n);
		float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

		bool cannotRefract = refraction * sinTheta > 1.0f;
		if (cannotRefract || Reflectance(cosTheta, refraction) > RandomFloat()) {
			scatteredDirection = glm::reflect(ray.d, collision.n);
		}
		else {
			scatteredDirection = glm::refract(ray.d, collision.n, refraction);
		}
	}
	else {
		Frame shadingFrame = Frame::FromZ(collision.n);
		scatteredDirection = shadingFrame.FromLocal(SampleUniformHemisphere(glm::vec2(RandomFloat(), RandomFloat())));
	}

	scatteredRay.o = collision.p;
	scatteredRay.d = scatteredDirection;
	float cosTheta = glm::abs(glm::dot(collision.n, scatteredRay.d));
	return (cosTheta * Evaluate(collision)) / Pdf();
}

BxDF* RTMaterial::GetBxDF(const RTInteraction& intr) const {
	return new DiffuseBxDF(texture->Sample(intr));
}

BSDF RTMaterial::GetBSDF(const RTInteraction& intr) const {
	BxDF* bxdf = GetBxDF(intr);
	return BSDF(intr.n, intr.dpdus, bxdf);
}
