#pragma once
#include "MathAndPhysics.h"
#include "Ray.h"
#include "Interaction.h"
#include "Bounds.h"

class Transform {
public:
	Transform() = default;
	Transform(const Vec3& position, const Quaternion& rotation = Quaternion(), const Vec3& scale = Vec3(1.0f));
	Transform(const Mat4& m);
	Transform(const Mat4& m, const Mat4& mInv);

	const Mat4& GetMatrix() const;
	const Mat4& GetInverseMatrix() const;
	Vec3 GetPosition() const;
	Quaternion GetRotation() const;
	Vec3 GetEulerRotation() const;
	Vec3 GetScale() const;
	Vec3 GetRight() const;
	Vec3 GetUp() const;
	Vec3 GetForward() const;

	void Set(const Transform& transform);
	void Set(Mat4 transform);
	void Set(const Vec3& position, const Quaternion& rotation = Quaternion(), const Vec3& scale = Vec3(1.0f));
	void LookAt(Vec3 pos, Vec3 look, Vec3 up);
	void SetPosition(const Vec3& pos);
	void Translate(const Vec3& offset);
	void SetRotation(const Quaternion& rotation);
	void SetEulerRotation(Vec3 degrees);
	void Rotate(const Quaternion& rotation);
	void Rotate(Float x, Float y, Float z);
	void RotateX(Float radians);
	void RotateY(Float radians);
	void RotateZ(Float radians);
	void RotateAroundAxis(Vec3 axis, Float radians);
	void SetScale(const Vec3& scale);
	void Scale(const Vec3& change);
	void Invert();

	Vec3 ApplyPoint(Vec3 p) const;
	Vec3 ApplyVector(Vec3 v) const;
	Vec3 ApplyNormal(Vec3 n) const;
	Ray ApplyRay(const Ray& r, Float* tMax = nullptr) const;
	RayInteraction ApplyInteraction(const RayInteraction& in) const;
	Bounds3f ApplyBounds(const Bounds3f& b) const;

	Vec3 ApplyInversePoint(Vec3 p) const;
	Vec3 ApplyInverseVector(Vec3 v) const;
	Vec3 ApplyInverseNormal(Vec3 n) const;
	Ray ApplyInverseRay(const Ray& r, Float* tMax = nullptr) const;
	RayInteraction ApplyInverseInteraction(const RayInteraction& in) const;

	bool IsIdentity() const;
	bool HasScale(Float tolerance = 1e-3f) const;
	bool SwapsHandedness() const;

	bool operator==(const Transform& t) const;
	bool operator!=(const Transform& t) const;
	Transform operator*(const Transform& t2) const;

protected:
	Mat4 m_transform = Mat4(1.0f);
	Mat4 m_inverseTransform = Mat4(1.0f);
};

Transform Inverse(const Transform& t);
Transform Transpose(const Transform& t);
Transform RotateFromTo(Vec3 from, Vec3 to);
Transform RotateAroundAxis(Float sinTheta, Float cosTheta, Vec3 axis);
Transform RotateAroundAxis(Float theta, Vec3 axis);
Transform LookAt(Vec3 from, Vec3 to, Vec3 up);

