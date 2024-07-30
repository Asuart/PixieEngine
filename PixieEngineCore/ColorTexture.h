#ifndef PIXIE_ENGINE_COLOR_TEXTURE
#define PIXIE_ENGINE_COLOR_TEXTURE

#include "pch.h"
#include "RTTexture.h"

class ColorTexture : public RTTexture {
public:
	ColorTexture(const glm::vec3& _color);
	glm::vec3 Sample(const RTInteraction& intr) const override;

private:
	glm::vec3 color;
};

#endif // PIXIE_ENGINE_COLOR_TEXTURE