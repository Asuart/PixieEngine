#include "ColorTexture.h"

ColorTexture::ColorTexture(const Vec3& _color)
	: color(_color) {}

Vec3 ColorTexture::Sample(const RTInteraction& intr) const {
	return color;
}