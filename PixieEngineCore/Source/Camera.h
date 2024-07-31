#pragma once
#include "headers.h"
#include "Transform.h"

class Camera {
public:
	Transform transform;
	glm::vec3 lookFrom = glm::vec3(0, 0, 0);
	glm::vec3 lookAt = glm::vec3(0, 1, 0);
	glm::vec3 up = glm::vec3(0, 0, 1);
	float fov = Pi;
	float aspect = 1;
	float near = ShadowEpsilon;
	float far = 10000.0;
	float lensRadius = 0;
	float focusDistance = 0;

	Camera(glm::vec3 _lookFrom, glm::vec3 _lookAt, glm::vec3 _up, float _fov, float _aspect, float _aperture, float _focusDistance, float _near = ShadowEpsilon, float _far = 10000.0);

	void LookAt(const glm::vec3& _lookFrom, const glm::vec3& _lookAt, const glm::vec3& _up);
	Ray GetRay(const glm::vec2& uv) const;
};
