#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Texture.h"
#include "RTTexture.h"
#include "BSDF.h"
#include "Ray.h"

struct SurfaceInteraction;

class Material {
public:
	const std::string name;
	glm::fvec3 albedo;
	glm::fvec3 emission;
	float roughness;
	float metallic;
	float transparency;
	float eta;
	Texture<glm::fvec3>* albedoTexture = nullptr;
	Texture<glm::fvec3>* normalMap = nullptr;
	Texture<float>* bumpMap = nullptr;
	RTTexture* rtTexture = nullptr;

	Material(const std::string& name, Vec3 _albedo = Vec3(0.8f), Vec3 _emission = Vec3(0.0f), Float _roughness = 1.0f, Float _metallic = 0.0f, Float _transparency = 0.0f, Float _eta = 1.0f);
	
	inline Vec3 Evaluate(const SurfaceInteraction& collision) const;
	inline constexpr Float Pdf() const;
	Vec3 Sample(const Ray& ray, const SurfaceInteraction& collision, Ray& scatteredRay) const;
	virtual BxDF* GetBxDF(const SurfaceInteraction& intr) const;
	BSDF GetBSDF(const SurfaceInteraction& intr) const;
};
