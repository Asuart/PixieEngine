#pragma once
#include "RTTexture.h"

class ColorTexture : public RTTexture {
public:
	ColorTexture(const Vec3& _color);
	Vec3 Sample(const RTInteraction& intr) const override;

private:
	Vec3 color;
};
