#pragma once
#include "pch.h"
#include "Ray.h"
#include "RTTexture.h"
#include "RTMath.h"
#include "BSDF.h"
#include "Material.h"

class RTMaterial : public Material {
public:
	RTTexture* texture;

	RTMaterial(const std::string& name, RTTexture* texture = nullptr) : Material(name), texture(texture) {}

	inline glm::vec3 Evaluate(const RTInteraction& collision) const;
	inline constexpr float Pdf() const;
	glm::vec3 Sample(const Ray& ray, const RTInteraction& collision, Ray& scatteredRay) const;
	virtual BxDF* GetBxDF(const RTInteraction& intr) const;
	BSDF GetBSDF(const RTInteraction& intr) const;
};
