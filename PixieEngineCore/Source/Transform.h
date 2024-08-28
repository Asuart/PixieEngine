#pragma once
#include "UID.h"
#include "RTMath.h"
#include "Ray.h"
#include "Interaction.h"
#include "Bounds.h"

class Transform {
public:
	UID id;

	Transform(const Vec3& position = Vec3(0.0f), const Vec3& rotation = Vec3(0.0f), const Vec3& scale = Vec3(1.0f));
	Transform(const Mat4& m);
	Transform(const Mat4& m, const Mat4& mInv);

	const Mat4& GetMatrix() const;
	const Mat4& GetInverseMatrix() const;
	Vec3& GetPosition();
	Vec3& GetRotation();
	Vec3& GetScale();
	const Vec3& GetPositionValue() const;
	const Vec3& GetRotationValue() const;
	const Vec3& GetScaleValue() const;


	void LookAt(Vec3 pos, Vec3 look, Vec3 up);
	void SetPosition(Float x, Float y, Float z);
	void SetPosition(const Vec3& pos);
	void Move(Float x, Float y, Float z);
	void Move(const Vec3& offset);
	void MoveForward(Float value);
	void MoveRight(Float value);
	void MoveUp(Float value);
	void SetRotation(Float x, Float y, Float z);
	void SetRotation(const Vec3& rotation);
	void SetRotationX(Float rotation);
	void SetRotationY(Float rotation);
	void SetRotationZ(Float rotation);
	void AddRotation(Float x, Float y, Float z);
	void AddRotationX(Float rotation);
	void AddRotationY(Float rotation);
	void AddRotationZ(Float rotation);
	void SetScale(Float x, Float y, Float z);
	void SetScale(const Vec3& scale);
	void AddScale(Float x, Float y, Float z);
	void AddScale(const Vec3& change);

	void UpdateMatrices();

	Vec3 ApplyPoint(Vec3 p) const;
	Vec3 ApplyVector(Vec3 v) const;
	Vec3 ApplyNormal(Vec3 n) const;
	Ray ApplyRay(const Ray& r, Float* tMax = nullptr) const;
	SurfaceInteraction ApplyInteraction(const SurfaceInteraction& in) const;
	Bounds3f ApplyBounds(const Bounds3f& b) const;

	Vec3 ApplyInversePoint(Vec3 p) const;
	Vec3 ApplyInverseVector(Vec3 v) const;
	Vec3 ApplyInverseNormal(Vec3 n) const;
	Ray ApplyInverseRay(const Ray& r, Float* tMax = nullptr) const;
	SurfaceInteraction ApplyInverseInteraction(const SurfaceInteraction& in) const;

	bool IsIdentity() const;
	bool HasScale(Float tolerance = 1e-3f) const;
	bool SwapsHandedness() const;

	bool operator==(const Transform& t) const;
	bool operator!=(const Transform& t) const;
	Transform operator*(const Transform& t2) const;

protected:
	Mat4 m_transform;
	Mat4 m_inverseTransform;
	Vec3 m_position;
	Vec3 m_rotation;
	Vec3 m_scale;
	Vec3 m_forward;
	Vec3 m_right;
	Vec3 m_up;

	void Decompose();
	void UpdateDirections();
};

Transform Inverse(const Transform& t);
Transform Transpose(const Transform& t);
Transform RotateFromTo(Vec3 from, Vec3 to);
Transform Rotate(Float sinTheta, Float cosTheta, Vec3 axis);
Transform Rotate(Float theta, Vec3 axis);
