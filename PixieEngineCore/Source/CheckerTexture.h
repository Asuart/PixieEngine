#pragma once
#include "RTTexture.h"

class CheckerTexture : public RTTexture {
public:
	CheckerTexture(RTTexture* _even, RTTexture* _odd, float scale = 10);
	glm::vec3 Sample(const RTInteraction& intr) const override;

private:
	RTTexture* even, * odd;
	float scale;
};
