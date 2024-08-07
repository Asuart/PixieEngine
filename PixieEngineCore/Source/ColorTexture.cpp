#include "ColorTexture.h"

ColorTexture::ColorTexture(const Vec3& materialColor)
	: color(materialColor) {}

Vec3 ColorTexture::Sample(const SurfaceInteraction& intr) const {
	return color;
}