#ifndef PIXIE_ENGINE_CORE_TRANSFORM
#define PIXIE_ENGINE_CORE_TRANSFORM

#include "pch.h"
#include "Math.h"
#include "Ray.h"
#include "RTInteraction.h"
#include "Bounds.h"

class Transform {
	glm::mat4 m = glm::mat4(), mInv = glm::mat4();
public:
	Transform();
	Transform(const glm::mat4& m);
	Transform(const glm::mat4& m, const glm::mat4& mInv);

	const glm::mat4& GetMatrix() const;
	const glm::mat4& GetInverseMatrix() const;

	glm::vec3 ApplyPoint(glm::vec3 p) const;
	glm::vec3 ApplyVector(glm::vec3 v) const;
	glm::vec3 ApplyNormal(glm::vec3 n) const;
	Ray ApplyRay(const Ray& r, float* tMax = nullptr) const;
	RTInteraction ApplyInteraction(const RTInteraction& in) const;
	Bounds3f ApplyBounds(const Bounds3f& b) const;

	glm::vec3 ApplyInversePoint(glm::vec3 p) const;
	glm::vec3 ApplyInverseVector(glm::vec3 v) const;
	glm::vec3 ApplyInverseNormal(glm::vec3 n) const;
	Ray ApplyInverseRay(const Ray& r, float* tMax = nullptr) const;
	RTInteraction ApplyInverseInteraction(const RTInteraction& in) const;

	void Decompose(glm::vec3* T, glm::mat4* R, glm::mat4* S) const;
	bool IsIdentity() const;
	bool HasScale(float tolerance = 1e-3f) const;
	bool SwapsHandedness() const;

	bool operator==(const Transform& t) const;
	bool operator!=(const Transform& t) const;
	Transform operator*(const Transform& t2) const;
};

Transform Inverse(const Transform& t);
Transform Transpose(const Transform& t);
Transform Rotate(float sinTheta, float cosTheta, glm::vec3 axis);
Transform Rotate(float theta, glm::vec3 axis);
Transform RotateFromTo(glm::vec3 from, glm::vec3 to);
Transform Translate(glm::vec3 delta);
Transform Scale(float x, float y, float z);
Transform RotateX(float theta);
Transform RotateY(float theta);
Transform RotateZ(float theta);
Transform LookAt(glm::vec3 pos, glm::vec3 look, glm::vec3 up);
Transform Orthographic(float zNear, float zFar);
Transform Perspective(float fov, float n, float f);

#endif // PIXIE_ENGINE_CORE_TRANSFORM