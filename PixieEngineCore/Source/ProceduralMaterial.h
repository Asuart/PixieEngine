#pragma once
#include "Material.h"
#include "ProceduralTexture.h"

struct MaterialSample {
	Spectrum f;
	Vec3 scattered;
	Float pdf;
	BxDFFlags flags;
	Float refraction;
	bool transmission = false;
};

struct ProceduralMaterial {
	ProceduralTexture<Spectrum>* m_albedo;
	ProceduralTexture<Spectrum>* m_emissionColor;
	ProceduralTexture<Float>* m_emissionStrength;
	ProceduralTexture<Float>* m_roughness;
	Float m_transparency;
	Float m_refraction;
	BxDFFlags m_flags;

	ProceduralMaterial(Material& material) {
		m_albedo = new UniformTexture<Spectrum>({ 1, 1 }, material.m_albedo);
		m_emissionColor = new UniformTexture<Spectrum>({ 1, 1 }, material.m_emissionColor);
		m_emissionStrength = new UniformTexture<Float>({ 1, 1 }, material.m_emissionStrength);
		m_roughness = new UniformTexture<Float>({ 1,1 }, material.m_roughness);
		m_transparency = material.m_transparency;
		m_refraction = material.m_refraction;
		m_flags = BxDFFlags::Diffuse;
	}

	~ProceduralMaterial() {
		delete m_albedo;
		delete m_emissionColor;
		delete m_emissionStrength;
		delete m_roughness;
	}

	BxDFFlags Flags() {
		return m_flags;
	}
};
