#pragma once
#include "Spectrum.h"
#include "Interaction.h"

class GBufferPixel {
public:
	Spectrum albedo = Spectrum(0.0f);
	Spectrum light = Spectrum(0.0f);
	Vec3 position = Vec3(0.0f);
	Vec3 normal = Vec3(0.0f, 0.0f, 1.0f);
	Vec2 uv = Vec2(0.0f, 0.0f);
	Float depth = 0.0f;
	int32_t shapeChecks = 0;
	int32_t boxChecks = 0;

	GBufferPixel() = default;
	GBufferPixel(const RayInteraction& si, Spectrum albedo, Float depth) :
		position(si.position), normal((glm::dot(si.normal, si.wo) < 0.f) ? -si.normal : si.normal), uv(si.uv), albedo(albedo), depth(depth) {}
};
