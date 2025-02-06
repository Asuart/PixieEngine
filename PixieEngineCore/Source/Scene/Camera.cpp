#include "pch.h"
#include "Camera.h"

Camera::Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 up, Float fovy, glm::ivec2 resolution, Float aperture, Float focusDistance, Float near, Float far) :
	m_fovy(fovy), m_resolution(resolution), m_aspect((Float)resolution.x / resolution.y), m_near(near), m_far(far), m_focusDistance(focusDistance), m_lensRadius(aperture / 2.0f) {
	LookAt(lookFrom, lookAt, up);
	UpdateProjection();
}

Camera::Camera(Transform transform, Float fovy, glm::ivec2 resolution, Float aperture, Float focusDistance, Float near, Float far) :
	m_transform(transform), m_resolution(resolution), m_fovy(fovy), m_aspect((Float)resolution.x / resolution.y), m_near(near), m_far(far), m_focusDistance(focusDistance), m_lensRadius(aperture / 2.0f) {
	UpdateProjection();
}

void Camera::LookAt(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up) {
	m_transform.LookAt(lookFrom, lookAt, up);
}

Ray Camera::GetRay(const Vec2& coord) const {
	Float theta = Lerp(coord.x, -(m_fovy * m_aspect) / 2, (m_fovy * m_aspect) / 2);
	Float phi = Lerp(coord.y, -m_fovy / 2, m_fovy / 2);
	Vec3 dir = glm::normalize(glm::mat3(m_transform.GetMatrix()) * Vec3(glm::tan(theta), glm::tan(phi), -1.0f));
	return Ray(m_transform.GetPosition(), dir);
}

Float Camera::GetFieldOfViewY() const {
	return m_fovy;
}

void Camera::SetFieldOfViewY(Float fovy) {
	m_fovy = fovy;
	UpdateProjection();
}

glm::ivec2 Camera::GetResolution() const {
	return m_resolution;
}

void Camera::SetResolution(glm::ivec2 resolution) {
	m_resolution = resolution;
	m_aspect = (Float)resolution.x / resolution.y;
	UpdateProjection();
}

Transform& Camera::GetTransform() {
	return m_transform;
}

const Transform& Camera::GetTransform() const {
	return m_transform;
}

const Mat4& Camera::GetViewMatrix() const {
	return m_transform.GetInverseMatrix();
}

const Mat4& Camera::GetInverseViewMatrix() const {
	return m_transform.GetMatrix();
}

const Mat4& Camera::GetProjectionMatrix() const {
	return m_mProjection;
}

void Camera::UpdateProjection() {
	m_mProjection = glm::perspective(m_fovy, m_aspect, m_near, m_far);
}

bool Camera::operator!=(const Camera& other) {
	return m_mProjection != other.m_mProjection || m_transform != other.m_transform || m_resolution != other.m_resolution;
}

bool Camera::operator==(const Camera& other) {
	return m_mProjection == other.m_mProjection && m_transform == other.m_transform && m_resolution == other.m_resolution;
}
