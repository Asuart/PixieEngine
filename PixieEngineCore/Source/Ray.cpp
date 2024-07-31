#include "Ray.h"

Ray::Ray(const glm::vec3& _origin, const glm::vec3& _direction)
	: o(_origin), d(_direction) {
	if (isnan(o) || isnan(d)) {
		std::cout << "nan ray\n";
	}
}

glm::vec3 Ray::At(float t) const {
	return o + d * t;
}