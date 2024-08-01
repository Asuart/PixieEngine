#include "RTMaterial.h"

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

inline Vec3 RTMaterial::Evaluate(const RTInteraction& collision) const {
	Vec3 color = albedo;
	if (texture) {
		color *= texture->Sample(collision);
	}
	return color / Pi;
}

inline constexpr Float RTMaterial::Pdf() const {
	return Inv2Pi;
}

Vec3 RTMaterial::Sample(const Ray& ray, const RTInteraction& collision, Ray& scatteredRay) const {
	Vec3 scatteredDirection;

	if (transparency > RandomFloat()) {
		Float refraction = !collision.backFace ? eta : (1.0f / eta);

		Float cosTheta = glm::dot(-ray.d, collision.n);
		Float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

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
		scatteredDirection = shadingFrame.FromLocal(SampleUniformHemisphere(Vec2(RandomFloat(), RandomFloat())));
	}

	scatteredRay.o = collision.p;
	scatteredRay.d = scatteredDirection;
	Float cosTheta = glm::abs(glm::dot(collision.n, scatteredRay.d));
	return (cosTheta * Evaluate(collision)) / Pdf();
}

BxDF* RTMaterial::GetBxDF(const RTInteraction& intr) const {
	return new DiffuseBxDF(texture->Sample(intr));
}

BSDF RTMaterial::GetBSDF(const RTInteraction& intr) const {
	BxDF* bxdf = GetBxDF(intr);
	return BSDF(intr.n, intr.dpdus, bxdf);
}
