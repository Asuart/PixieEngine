#pragma once
#include "pch.h"
#include "MathAndPhysics.h"

class TrowbridgeReitzDistribution {
public:
	TrowbridgeReitzDistribution(Float alpha_x, Float alpha_y);

	Float D(Vec3 wm) const;
	bool EffectivelySmooth() const;
	Float G1(Vec3 w) const;
	Float Lambda(Vec3 w) const;
	Float G(Vec3 wo, Vec3 wi) const;
	Float D(Vec3 w, Vec3 wm) const;
	Float PDF(Vec3 w, Vec3 wm) const;
	Vec3 Sample_wm(Vec3 w, Vec2 u) const;
	static Float RoughnessToAlpha(Float roughness);
	void Regularize();

private:
	Float alpha_x, alpha_y;
};
