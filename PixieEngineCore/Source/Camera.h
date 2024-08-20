#pragma once
#include "pch.h"
#include "Transform.h"

class Camera {
public:
	Transform m_transform;

	Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 up, Float fovy, Float aspect, Float aperture = 0, Float focusDistance = 0, Float _near = 0.0001f, Float _far = 10000.0);

	void LookAt(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up);
	void SetAspect(float aspect);
	void SetFieldOfViewY(float fovy);
	Ray GetRay(uint32_t x, uint32_t y, const Vec2& uv) const;

	const Mat4& GetViewMatrix() const;
	const Mat4& GetInverseViewMatrix() const;
	const Mat4& GetProjectionMatrix() const;

protected:
	Float m_fovy = Pi / 2.0f;
	Float m_aspect = 1.0f;
	Float m_near = 0.0001f;
	Float m_far = 10000.0f;
	Float m_lensRadius = 0;
	Float m_focusDistance = 0;
	Mat4 m_mProjection = Mat4(1.0);

	void UpdateProjection();
};
