#include "pch.h"
#include "ShapeSampleContext.h"

ShapeSampleContext::ShapeSampleContext(const Vec3& position, const Vec3& normal)
	: position(position), normal(normal) {}

ShapeSampleContext::ShapeSampleContext(const SurfaceInteraction& intr)
	: position(intr.position), normal(intr.normal) {}