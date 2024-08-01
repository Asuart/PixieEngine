#include "CheckerTexture.h"

CheckerTexture::CheckerTexture(RTTexture* _even, RTTexture* _odd, Float scale)
	: even(_even), odd(_odd), scale(scale) {}

Vec3 CheckerTexture::Sample(const RTInteraction& intr) const {
	Float sines = sin(scale * intr.p.x) * sin(scale * intr.p.y) * sin(scale * intr.p.z);
	if (sines < 0) {
		return odd->Sample(intr);
	}
	else {
		return even->Sample(intr);
	}
}