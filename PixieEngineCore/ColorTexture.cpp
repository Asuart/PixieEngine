#include "pch.h"
#include "ColorTexture.h"

ColorTexture::ColorTexture(const glm::vec3& _color)
	: color(_color) {}

glm::vec3 ColorTexture::Sample(const RTInteraction& intr) const {
	return color;
}