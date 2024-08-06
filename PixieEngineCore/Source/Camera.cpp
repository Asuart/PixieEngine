
#include "Camera.h"

Camera::Camera(Vec3 _lookFrom, Vec3 _lookAt, Vec3 _up, Float _fov, Float _aspect, Float _aperture, Float _focusDistance, Float _near, Float _far)
	: fov(_fov), aspect(_aspect), near(_near), focusDistance(_focusDistance), lensRadius(_aperture / 2.0f), far(_far) {
	LookAt(_lookFrom, _lookAt, _up);
}

void Camera::LookAt(const Vec3& _lookFrom, const Vec3& _lookAt, const Vec3& _up) {
	lookAt = _lookAt;
	lookFrom = _lookFrom;
	up = _up;
	transform = ::LookAt(_lookFrom, _lookAt, _up);
}

Ray Camera::GetRay(uint32_t x, uint32_t y, const Vec2& coord) const {
	Float theta = Lerp(coord.x, (fov * aspect) / 2, -(fov * aspect) / 2);
	Float phi = Lerp(coord.y, -fov / 2, fov / 2);
	Vec3 dir = glm::normalize(Vec3(glm::tan(theta), glm::tan(phi), 1));
	return transform.ApplyInverseRay(Ray(x, y, Vec3(0), dir));
}