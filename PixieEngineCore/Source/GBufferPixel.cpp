#include "pch.h"
#include "GBufferPixel.h"

GBufferPixel::GBufferPixel(const SurfaceInteraction& si, Spectrum albedo, Float depth)
	: p(si.position), n((glm::dot(si.normal, si.wo) < 0.f) ? -si.normal : si.normal), uv(si.uv), albedo(albedo), depth(depth), set(true) {}

GBufferPixel::operator bool() const {
	return set; 
}
