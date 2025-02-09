#pragma once
#include "pch.h"
#include "Buffer2DTexture.h"
#include "Math/Random.h"
#include "RayTracing/BSDF.h"
#include "RayTracing/Ray.h"
#include "RayTracing/Spectrum.h"
#include "RayTracing/Samplers.h"

struct RayInteraction;

struct Material {
	std::string m_name;
	Spectrum m_albedo = Spectrum(0.8f);
	Buffer2DTexture<Vec3> m_albedoTexture;
	Spectrum m_emissionColor = Spectrum(1.0f);
	Float m_emissionStrength = 0.0f;
	Buffer2DTexture<Vec3> m_emissionTexture;
	Float m_metallic = 1.0f;
	Buffer2DTexture<Float> m_metallicTexture;
	Float m_roughness = 1.0f;
	Buffer2DTexture<Float> m_roughnessTexture;
	Float m_refraction = 1.0f;
	Float m_transparency = 0.0f;
	Buffer2DTexture<Vec3> m_normalTexture;
	Buffer2DTexture<Float> m_aoTexture;

	Material(const std::string& name = "Unnamed Material", Spectrum albedo = Spectrum(0.8f), Spectrum emissionColor = Spectrum(1.0f), float emissionStrength = 0.0f, float roughness = 1.0f, float metallic = 0.0f, float transparency = 0.0f, float refraction = 1.0f);

	bool IsEmissive();
	bool IsTranslucent();
	Spectrum GetEmission();
	Spectrum Evaluate(const RayInteraction& intr);
	BSDF GetBSDF(const RayInteraction& intr);
	BxDFFlags Flags();
	Float Pdf();
};
