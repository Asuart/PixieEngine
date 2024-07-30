#include "pch.h"
#include "VisibleSurface.h"

VisibleSurface::VisibleSurface(const RTInteraction& si, glm::vec3 albedo)
	: p(si.p), n((glm::dot(si.n, si.wo) < 0.f) ? -si.n : si.n), uv(si.uv), albedo(albedo), set(true) {}

VisibleSurface::operator bool() const { 
	return set; 
}