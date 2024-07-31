#include "CheckerTexture.h"

CheckerTexture::CheckerTexture(RTTexture* _even, RTTexture* _odd, float scale)
	: even(_even), odd(_odd), scale(scale) {}

glm::vec3 CheckerTexture::Sample(const RTInteraction& intr) const {
	float sines = sin(scale * intr.p.x) * sin(scale * intr.p.y) * sin(scale * intr.p.z);
	if (sines < 0) {
		return odd->Sample(intr);
	}
	else {
		return even->Sample(intr);
	}
}