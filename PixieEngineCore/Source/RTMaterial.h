#pragma once
#include "RTMath.h"
#include "Ray.h"
#include "RTTexture.h"
#include "BSDF.h"
#include "Material.h"

class RTMaterial : public Material {
public:
	RTTexture* texture;

	RTMaterial(const std::string& name, RTTexture* texture = nullptr) : Material(name), texture(texture) {}

	inline Vec3 Evaluate(const RTInteraction& collision) const;
	inline constexpr Float Pdf() const;
	Vec3 Sample(const Ray& ray, const RTInteraction& collision, Ray& scatteredRay) const;
	virtual BxDF* GetBxDF(const RTInteraction& intr) const;
	BSDF GetBSDF(const RTInteraction& intr) const;
};
