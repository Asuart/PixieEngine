#include "pch.h"
#include "RandomWalkIntegrator.h"
#include "SceneSnapshot.h"

Spectrum RandomWalkIntegrator::SampleLightRay(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler) {
	return IntegrateRandomWalk(sceneSnapshot, ray, sampler);
}

Spectrum RandomWalkIntegrator::IntegrateRandomWalk(SceneSnapshot* sceneSnapshot, Ray ray, Sampler* sampler, uint32_t depth) {
	RayTracingStatistics::IncrementRays();
	std::optional<ShapeIntersection> si = RayTracing::Intersect(ray, sceneSnapshot);
	if (!si) {
		Spectrum Le;
		for (Light* light : sceneSnapshot->GetInfiniteLights()) {
			Le += light->Le(ray);
		}
		return Le;
	}

	Vec3 wo = -ray.direction;
	SurfaceInteraction intr = si->intr;
	Spectrum Le = intr.Le(wo);

	if (depth == RayTracing::c_maxRayBounces) {
		return Le;
	}

	BSDF bsdf = intr.GetBSDF(ray, sampler);

	Vec3 wp = SampleUniformSphere(sampler->Get2D());

	Spectrum fcos = bsdf.SampleDistribution(wo, wp) * AbsDot(wp, intr.normal);
	if (!fcos) {
		return Le;
	}

	return Le + fcos * IntegrateRandomWalk(sceneSnapshot, Ray(intr.position, wp), sampler, depth + 1) / (1 / (4 * Pi));
}
