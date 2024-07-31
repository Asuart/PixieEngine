#pragma once
#include "RTTexture.h"

class ColorTexture : public RTTexture {
public:
	ColorTexture(const glm::vec3& _color);
	glm::vec3 Sample(const RTInteraction& intr) const override;

private:
	glm::vec3 color;
};
