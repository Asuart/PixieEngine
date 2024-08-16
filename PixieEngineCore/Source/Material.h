#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Texture.h"
#include "RTTexture.h"
#include "BSDF.h"
#include "Ray.h"
#include "Spectrum.h"

struct SurfaceInteraction;

class Material {
public:
	Spectrum m_albedo = Spectrum(0.8f, 0.8f, 0.8f);
	Texture<Spectrum>* m_albedoTexture = nullptr;
	Spectrum m_emission = Spectrum(0.0f, 0.0f, 0.0f);
	Texture<Spectrum>* m_emissionTexture = nullptr;
	float m_metallic = 1.0f;
	Texture<float>* m_metallicTexture = nullptr;
	float m_roughness = 1.0f;
	Texture<float>* m_roughnessTexture = nullptr;
	float m_ambiendOcclusion = 1.0f;
	Texture<float>* m_ambientOcclusionTexture = nullptr;
	float m_refraction = 1.0f;
	float m_transparency = 0.0f;
	Texture<glm::fvec3>* m_normalTexture = nullptr;

	bool IsEmissive();
	bool IsTranslucent();

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
