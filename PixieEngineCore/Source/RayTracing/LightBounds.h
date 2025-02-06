#pragma once
#include "pch.h"
#include "Math/Bounds.h"
#include "Math/DirectionCone.h"

struct LightBounds {
    Bounds3f bounds;
    Vec3 w = Vec3(0);
    Float phi = 0.0f;
    Float cosTheta_o = 0.0f;
    Float cosTheta_e = 0.0f;

    LightBounds(const Bounds3f& b, Vec3 w, Float phi, Float cosTheta_o,  Float cosTheta_e);

    Vec3 Centroid() const;
    Float Importance(Vec3 p, Vec3 n) const;
};

LightBounds Union(const LightBounds& a, const LightBounds& b);
