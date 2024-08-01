#include "Ray.h"

Ray::Ray(const Vec3& _origin, const Vec3& _direction)
	: o(_origin), d(_direction) {
	if (isnan(o) || isnan(d)) {
		std::cout << "nan ray\n";
	}
}

Vec3 Ray::At(Float t) const {
	return o + d * t;
}