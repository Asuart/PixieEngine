#include "RandomWalkIntegrator.h"

RandomWalkIntegrator::RandomWalkIntegrator(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Vec3 RandomWalkIntegrator::Integrate(Ray ray, Sampler* sampler) {
	return IntegrateRandomWalk(ray, sampler);
}

Vec3 RandomWalkIntegrator::IntegrateRandomWalk(Ray ray, Sampler* sampler, uint32_t depth) {
	SurfaceInteraction isect;

	if (!m_scene->Intersect(ray, isect, m_stats)) {
		return m_scene->GetSkyColor(ray);
	}

	Vec3 wo = -ray.direction;
	glm::vec3 Le = isect.Le(wo);

	if (depth == m_maxDepth) {
		return Le;
	}

	BSDF bsdf = isect.material->GetBSDF(isect);

	Vec2 u = sampler->Get2D();
	Vec3 wp = SampleUniformSphere(u);

	glm::vec3 fcos = bsdf.f(wo, wp) * glm::abs(glm::dot(wp, isect.normal));
	if (fcos == glm::vec3(0.0f)) {
		return Le;
	}

	ray = Ray(ray.x, ray.y, isect.position, wp);
	return Le + fcos * IntegrateRandomWalk(ray, sampler, depth + 1) / (1 / (4 * Pi));
}
