#include "Skybox.h"

SolidColorSkybox::SolidColorSkybox(glm::fvec3 color)
	: m_color(color) {}

glm::fvec3 SolidColorSkybox::GetColor(const Ray& ray) {
	return m_color;
}

GradientSkybox::GradientSkybox(glm::fvec3 color1, glm::fvec3 color2)
	: m_color1(color1), m_color2(color2) {}

glm::fvec3 GradientSkybox::GetColor(const Ray& ray) {
	Float a = 0.5f * (ray.direction.y + 1.0f);
	return (1.0f - a) * m_color1 + a * m_color2;
}