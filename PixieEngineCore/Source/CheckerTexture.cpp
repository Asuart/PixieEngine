#include "CheckerTexture.h"

CheckerTexture::CheckerTexture(RTTexture* _even, RTTexture* _odd, Float scale)
	: even(_even), odd(_odd), scale(scale) {}

Vec3 CheckerTexture::Sample(const SurfaceInteraction& intr) const {
	Float sines = sin(scale * intr.position.x) * sin(scale * intr.position.y) * sin(scale * intr.position.z);
	if (sines < 0.0f) {
		return odd->Sample(intr);
	}
	else {
		return even->Sample(intr);
	}
}