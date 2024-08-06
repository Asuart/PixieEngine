#include "NormalsTracer.h"

NormalsTracer::NormalsTracer(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Vec3 NormalsTracer::Integrate(Ray ray) {
	SurfaceInteraction intr;
	m_scene->Intersect(ray, intr, m_stats);
	return glm::abs(intr.normal);
}