#pragma once
#include "RTTexture.h"

class CheckerTexture : public RTTexture {
public:
	CheckerTexture(RTTexture* _even, RTTexture* _odd, Float scale = 10);
	Vec3 Sample(const SurfaceInteraction& intr) const override;

private:
	RTTexture* even, * odd;
	Float scale;
};
