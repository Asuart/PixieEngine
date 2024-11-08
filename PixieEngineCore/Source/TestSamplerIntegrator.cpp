#include "pch.h"
#include "TestSamplerIntegrator.h"

Spectrum TestSamplerIntegrator::SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) {
	Float u = sampler->Get1D();
	return Spectrum(u, u, u);
}
