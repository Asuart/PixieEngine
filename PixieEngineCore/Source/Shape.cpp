#include "pch.h"
#include "Shape.h"

ShapeSampleContext::ShapeSampleContext(const Vec3& position, const Vec3& normal)
	: position(position), normal(normal) {}

ShapeSampleContext::ShapeSampleContext(const SurfaceInteraction& intr)
	: position(intr.position), normal(intr.normal) {}

ShapeSample::ShapeSample(SurfaceInteraction intr, Float p)
	: intr(intr), p(p) {}
