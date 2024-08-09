
#include "Camera.h"

Camera::Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 up, Float fovy, Float aspect, Float aperture, Float focusDistance, Float near, Float far)
	: m_fovy(fovy), m_aspect(aspect), m_near(near), m_far(far), m_focusDistance(focusDistance), m_lensRadius(aperture / 2.0f) {
	LookAt(lookFrom, lookAt, up);
	UpdateProjection();
}

void Camera::LookAt(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up) {
	m_lookAt = lookAt;
	m_lookFrom = lookFrom;
	m_up = up;
	Mat4 look = glm::lookAt(lookFrom, lookAt, up);
	m_transform = Transform(look);
}

void Camera::SetAspect(float aspect) {
	m_aspect = aspect;
	UpdateProjection();
}

void Camera::SetFieldOfViewY(float fovy) {
	m_fovy = fovy;
	UpdateProjection();
}

Ray Camera::GetRay(uint32_t x, uint32_t y, const Vec2& coord) const {
	Float theta = Lerp(coord.x, (m_fovy * m_aspect) / 2, -(m_fovy * m_aspect) / 2);
	Float phi = Lerp(coord.y, -m_fovy / 2, m_fovy / 2);
	Vec3 dir = glm::normalize(glm::mat3(m_transform.GetMatrix()) * Vec3(glm::tan(theta), glm::tan(phi), 1.0f));
	return Ray(x, y, m_lookFrom, dir);
}

const Mat4& Camera::GetViewMatrix() const {
	return m_transform.GetMatrix();
}

const Mat4& Camera::GetProjectionMatrix() const {
	return m_mProjection;
}

void Camera::UpdateProjection() {
	m_mProjection = glm::perspective(m_fovy, m_aspect, m_near, m_far);
}