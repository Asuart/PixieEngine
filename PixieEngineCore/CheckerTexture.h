#ifndef PIXIE_ENGINE_CHECKER_TEXTURE
#define PIXIE_ENGINE_CHECKER_TEXTURE

#include "pch.h"
#include "RTTexture.h"

class CheckerTexture : public RTTexture {
public:
	CheckerTexture(RTTexture* _even, RTTexture* _odd, float scale = 10);
	glm::vec3 Sample(const RTInteraction& intr) const override;

private:
	RTTexture* even, * odd;
	float scale;
};

#endif // PIXIE_ENGINE_CHECKER_TEXTURE