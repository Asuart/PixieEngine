#include "pch.h"
#include "DirectionCone.h"

namespace PixieEngine {

DirectionCone::DirectionCone(glm::vec3 w, float cosTheta) :
	w(w), cosTheta(cosTheta) {
}

DirectionCone DirectionCone::EntireSphere() {
	return DirectionCone(glm::vec3(0, 0, 1), -1);
}

bool DirectionCone::IsEmpty() const {
	return cosTheta == Infinity;
}

bool Inside(const DirectionCone& d, glm::vec3 w) {
	return !d.IsEmpty() && glm::dot(d.w, glm::normalize(w)) >= d.cosTheta;
}

}