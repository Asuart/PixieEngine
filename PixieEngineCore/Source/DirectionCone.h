#pragma once
#include "pch.h"
#include "MathAndPhysics.h"
#include "Transform.h"

struct DirectionCone {
	Vec3 w = Vec3(0);
	Float cosTheta = Infinity;

	DirectionCone() = default;
	DirectionCone(Vec3 w, Float cosTheta);
	static DirectionCone EntireSphere();

	bool IsEmpty() const;
};

bool Inside(const DirectionCone& d, Vec3 w);
DirectionCone Union(const DirectionCone& a, const DirectionCone& b);
