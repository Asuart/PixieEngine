#include "pch.h"
#include "LightLiSample.h"

LightLiSample::LightLiSample(Spectrum L, Vec3 wi, Float pdf, const SurfaceInteraction& pLight)
	: L(L), wi(wi), pdf(pdf), pLight(pLight) {}