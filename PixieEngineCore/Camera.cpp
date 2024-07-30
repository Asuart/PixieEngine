#include "pch.h"
#include "Camera.h"

Camera::Camera(glm::vec3 _lookFrom, glm::vec3 _lookAt, glm::vec3 _up, float _fov, float _aspect, float _aperture, float _focusDistance, float _near, float _far)
	: fov(_fov), aspect(_aspect), near(_near), focusDistance(_focusDistance), lensRadius(_aperture / 2.0f), far(_far) {
	LookAt(_lookFrom, _lookAt, _up);
}

void Camera::LookAt(const glm::vec3& _lookFrom, const glm::vec3& _lookAt, const glm::vec3& _up) {
	lookAt = _lookAt;
	lookFrom = _lookFrom;
	up = _up;
	transform = ::LookAt(_lookFrom, _lookAt, _up);
}

Ray Camera::GetRay(const glm::vec2& coord) const {
	float theta = Lerp(coord.x, (fov * aspect) / 2, -(fov * aspect) / 2);
	float phi = Lerp(coord.y, -fov / 2, fov / 2);
	glm::vec3 dir = glm::normalize(glm::vec3(glm::tan(theta), glm::tan(phi), 1));
	return transform.ApplyInverseRay(Ray(glm::vec3(0), dir));
}