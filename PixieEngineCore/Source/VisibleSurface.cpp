
#include "VisibleSurface.h"

VisibleSurface::VisibleSurface(const SurfaceInteraction& si, Spectrum albedo)
	: p(si.position), n((glm::dot(si.normal, si.wo) < 0.f) ? -si.normal : si.normal), uv(si.uv), albedo(albedo), set(true) {}

VisibleSurface::operator bool() const { 
	return set; 
}