#include "ColorTexture.h"

ColorTexture::ColorTexture(const Vec3& _color)
	: color(_color) {}

Vec3 ColorTexture::Sample(const SurfaceInteraction& intr) const {
	return color;
}