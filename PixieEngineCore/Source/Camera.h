#pragma once
#include "PixieEngineCoreHeaders.h"
#include "Transform.h"

class Camera {
public:
	Transform transform;
	Vec3 lookFrom = Vec3(0, 0, 0);
	Vec3 lookAt = Vec3(0, 1, 0);
	Vec3 up = Vec3(0, 0, 1);
	Float fov = Pi;
	Float aspect = 1;
	Float near = ShadowEpsilon;
	Float far = 10000.0;
	Float lensRadius = 0;
	Float focusDistance = 0;

	Camera(Vec3 _lookFrom, Vec3 _lookAt, Vec3 _up, Float _fov, Float _aspect, Float _aperture, Float _focusDistance, Float _near = ShadowEpsilon, Float _far = 10000.0);

	void LookAt(const Vec3& _lookFrom, const Vec3& _lookAt, const Vec3& _up);
	Ray GetRay(const Vec2& uv) const;
};
