#include "pch.h"
#include "LightSampleContext.h"

LightSampleContext::LightSampleContext(const Vec3& position, const Vec3& normal)
	: position(position), normal(normal) {}

LightSampleContext::LightSampleContext(const SurfaceInteraction& intr)
	: position(intr.position), normal(intr.normal) {}