#include "pch.h"
#include "RandomWalkIntegrator.h"

RandomWalkIntegrator::RandomWalkIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Spectrum RandomWalkIntegrator::Integrate(Ray ray, Sampler* sampler) {
	return IntegrateRandomWalk(ray, sampler);
}

Spectrum RandomWalkIntegrator::IntegrateRandomWalk(Ray ray, Sampler* sampler, uint32_t depth) {
	SurfaceInteraction isect;
	if (!m_scene->Intersect(ray, isect, m_stats)) {
		Spectrum Le;
		for (Light* light : m_scene->GetInfiniteLights()) {
			Le += light->Le(ray);
		}
		return Le;
	}

	Vec3 wo = -ray.direction;
	Spectrum Le = isect.Le(wo);

	if (depth == m_maxDepth) {
		return Le;
	}

	BSDF bsdf = isect.GetBSDF(ray, m_scene->GetMainCamera(), sampler);

	Vec3 wp = SampleUniformSphere(sampler->Get2D());

	Spectrum fcos = bsdf.SampleDistribution(isect.triangle->shadingFrame, wo, wp) * AbsDot(wp, isect.normal);
	if (!fcos) {
		return Le;
	}

	ray = Ray(ray.x, ray.y, isect.position + isect.normal * ShadowEpsilon, wp);
	return Le + fcos * IntegrateRandomWalk(ray, sampler, depth + 1) / (1 / (4 * Pi));
}
