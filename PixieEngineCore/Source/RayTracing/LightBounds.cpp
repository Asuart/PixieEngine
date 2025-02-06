#include "pch.h"
#include "LightBounds.h"

LightBounds::LightBounds(const Bounds3f& b, Vec3 w, Float phi, Float cosTheta_o, Float cosTheta_e) :
    bounds(b), w(w), phi(phi), cosTheta_o(cosTheta_o), cosTheta_e(cosTheta_e) {}

Vec3 LightBounds::Centroid() const { 
	return (bounds.min + bounds.max) / (Float)2;
}

Float LightBounds::Importance(Vec3 p, Vec3 n) const {
    return 0;
}

LightBounds Union(const LightBounds& a, const LightBounds& b) {
    if (a.phi == 0) return b;
    if (b.phi == 0) return a;

    DirectionCone cone = Union(DirectionCone(a.w, a.cosTheta_o), DirectionCone(b.w, b.cosTheta_o));
    Float cosTheta_o = cone.cosTheta;
    Float cosTheta_e = std::min(a.cosTheta_e, b.cosTheta_e);

    return LightBounds(Union(a.bounds, b.bounds), cone.w, a.phi + b.phi, cosTheta_o, cosTheta_e);
}
