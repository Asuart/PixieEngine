#include "pch.h"
#include "RandomWalkIntegrator.h"

RandomWalkIntegrator::RandomWalkIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Spectrum RandomWalkIntegrator::Integrate(Ray ray, Sampler* sampler) {
	return IntegrateRandomWalk(ray, sampler);
}

Spectrum RandomWalkIntegrator::IntegrateRandomWalk(Ray ray, Sampler* sampler, uint32_t depth) {
	RayTracingStatistics::IncrementRays();
	std::optional<ShapeIntersection> si = m_scene->Intersect(ray);
	if (!si) {
		Spectrum Le;
		for (Light* light : m_scene->GetInfiniteLights()) {
			Le += light->Le(ray);
		}
		return Le;
	}

	Vec3 wo = -ray.direction;
	SurfaceInteraction intr = si->intr;
	Spectrum Le = intr.Le(wo);

	if (depth == m_maxDepth) {
		return Le;
	}

	BSDF bsdf = intr.GetBSDF(ray, m_scene->GetMainCamera(), sampler);

	Vec3 wp = SampleUniformSphere(sampler->Get2D());

	Spectrum fcos = bsdf.SampleDistribution(wo, wp) * AbsDot(wp, intr.normal);
	if (!fcos) {
		return Le;
	}

	ray = Ray(intr.position, wp);
	return Le + fcos * IntegrateRandomWalk(ray, sampler, depth + 1) / (1 / (4 * Pi));
}
