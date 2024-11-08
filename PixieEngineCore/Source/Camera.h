#pragma once
#include "pch.h"
#include "Transform.h"

struct CameraSample {
	Vec2 pFilm = Vec2(0); // point on film
	Float filterWeight = 1.0f;

	CameraSample(Vec2 p, Float weight) : pFilm(p), filterWeight(weight) {}
};

class Camera {
public:

	Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 up, Float fovy, Float aspect, Float aperture = 0, Float focusDistance = 0, Float _near = 0.01f, Float _far = 1000.0);

	void LookAt(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up);
	void SetAspect(float aspect);
	void SetFieldOfViewY(float fovy);
	Ray GetRay(const Vec2& uv) const;
	Float GetFieldOfView();

	Transform& GetTransform();
	const Transform& GetTransform() const;
	const Mat4& GetViewMatrix() const;
	const Mat4& GetInverseViewMatrix() const;
	const Mat4& GetProjectionMatrix() const;

protected:
	Transform m_transform;
	Float m_fovy = Pi / 2.0f;
	Float m_aspect = 1.0f;
	Float m_near = 0.01f;
	Float m_far = 1000.0f;
	Float m_lensRadius = 0;
	Float m_focusDistance = 0;
	Mat4 m_mProjection = Mat4(1.0);

	void UpdateProjection();
};
