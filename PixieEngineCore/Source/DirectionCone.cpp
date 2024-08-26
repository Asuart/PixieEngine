#include "pch.h"
#include "DirectionCone.h"

DirectionCone::DirectionCone(Vec3 w, Float cosTheta)
	: w(w), cosTheta(cosTheta) {}

DirectionCone DirectionCone::EntireSphere() {
	return DirectionCone(Vec3(0, 0, 1), -1);
}

bool DirectionCone::IsEmpty() const {
	return cosTheta == Infinity;
}

bool Inside(const DirectionCone& d, Vec3 w) {
	return !d.IsEmpty() && glm::dot(d.w, glm::normalize(w)) >= d.cosTheta;
}

DirectionCone Union(const DirectionCone& a, const DirectionCone& b) {
    if (a.IsEmpty()) return b;
    if (b.IsEmpty()) return a;

    Float theta_a = SafeACos(a.cosTheta), theta_b = SafeACos(b.cosTheta);
    Float theta_d = AngleBetween(a.w, b.w);
    if (std::min(theta_d + theta_b, Pi) <= theta_a) {
        return a;
    }
    if (std::min(theta_d + theta_a, Pi) <= theta_b) {
        return b;
    }

    Float theta_o = (theta_a + theta_d + theta_b) / 2;
    if (theta_o >= Pi) {
        return DirectionCone::EntireSphere();
    }

    Float theta_r = theta_o - theta_a;
    Vec3 wr = glm::cross(a.w, b.w);
    if (length2(wr) == 0) {
        return DirectionCone::EntireSphere();
    }

    Vec3 w = Rotate(glm::degrees(theta_r), wr) * a.w;
    return DirectionCone(w, std::cos(theta_o));
}
