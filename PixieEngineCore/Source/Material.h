#pragma once
#include "pch.h"
#include "Texture.h"
#include "Random.h"
#include "BSDF.h"
#include "Ray.h"
#include "Spectrum.h"
#include "Samplers.h"

struct RayInteraction;

struct Material {
	const std::string m_name;
	Spectrum m_albedo = Spectrum(0.8f);
	Texture<Vec3>* m_albedoTexture = nullptr;
	Spectrum m_emissionColor = Spectrum(1.0f);
	Float m_emissionStrength = 0.0f;
	Texture<Vec3>* m_emissionTexture = nullptr;
	Float m_metallic = 1.0f;
	Texture<float>* m_metallicTexture = nullptr;
	Float m_roughness = 1.0f;
	Texture<float>* m_roughnessTexture = nullptr;
	Float m_refraction = 1.0f;
	Float m_transparency = 0.0f;
	Texture<Vec3>* m_normalTexture = nullptr;

	Material(const std::string& name, Spectrum albedo = Spectrum(0.8f), Spectrum emissionColor = Spectrum(1.0f), float emissionStrength = 0.0f, float roughness = 1.0f, float metallic = 0.0f, float transparency = 0.0f, float refraction = 1.0f);

	bool IsEmissive();
	bool IsTranslucent();
	Spectrum GetEmission();
	Spectrum Evaluate(const RayInteraction& intr);
	BSDF GetBSDF(const RayInteraction& intr);
	Float Pdf();
};
