#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Texture.h"
#include "Random.h"
#include "BSDF.h"
#include "Ray.h"
#include "Spectrum.h"

struct SurfaceInteraction;

struct Material {
	const std::string m_name;
	Spectrum m_albedo = Spectrum(0.8f, 0.8f, 0.8f);
	Texture<Spectrum>* m_albedoTexture = nullptr;
	Spectrum m_emissionColor = Spectrum(1.0f, 1.0f, 1.0f);
	float m_emissionStrength = 0.0f;
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

	Material(const std::string& name, Spectrum albedo = Spectrum(0.8f), Spectrum emissionColor = Spectrum(0.0f), float emissionStrength = 0.0f, float roughness = 1.0f, float metallic = 0.0f, float transparency = 0.0f, float refraction = 1.0f);

	bool IsEmissive();
	bool IsTranslucent();
	Spectrum GetEmission() const;
	Spectrum Sample(const Ray& ray, const SurfaceInteraction& collision, Ray& scatteredRay) const;
	Spectrum Evaluate(const SurfaceInteraction& collision) const;
	BSDF GetBSDF(const SurfaceInteraction& intr) const;
	float Pdf() const;

protected:
	virtual BxDF* GetBxDF(const SurfaceInteraction& intr) const;
};
