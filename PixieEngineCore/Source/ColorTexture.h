#pragma once
#include "RTTexture.h"

class ColorTexture : public RTTexture {
public:
	ColorTexture(const Vec3& materialColor);
	Vec3 Sample(const SurfaceInteraction& intr) const override;

private:
	const Vec3& color;
};
