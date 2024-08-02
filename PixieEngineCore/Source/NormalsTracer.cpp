#include "NormalsTracer.h"

NormalsTracer::NormalsTracer(const glm::ivec2& resolution)
	: Integrator(resolution) {}

Vec3 NormalsTracer::Integrate(Ray ray) {
	RTInteraction intr;
	m_scene->Intersect(ray, intr);
	return glm::abs(intr.n);
}