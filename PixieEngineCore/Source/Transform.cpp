#include "pch.h"
#include "Transform.h"

struct Decomposition {
	Vec3 scale;
	Quaternion orientation;
	Vec3 translation;
	Vec3 skew;
	Vec4 perspective;
};

inline Decomposition Decompose(const Mat4& m) {
	Decomposition dec;
	glm::decompose(m, dec.scale, dec.orientation, dec.translation, dec.skew, dec.perspective);
	return dec;
}

inline Mat4 Recompose(const Decomposition& dec) {
	return glm::recompose(dec.scale, dec.orientation, dec.translation, dec.skew, dec.perspective);
}

Transform::Transform(const Vec3& position, const Quaternion& rotation, const Vec3& scale) {
	Set(position, rotation, scale);
};

Transform::Transform(const Mat4& m)
	: m_transform(m), m_inverseTransform(glm::inverse(m)) {}

Transform::Transform(const Mat4& m, const Mat4& mInv)
	: m_transform(m), m_inverseTransform(mInv) {}

const Mat4& Transform::GetMatrix() const {
	return m_transform;
}

const Mat4& Transform::GetInverseMatrix() const {
	return m_inverseTransform;
}

Vec3 Transform::GetPosition() const {
	Decomposition dec = Decompose(m_transform);
	return dec.translation;
}

Quaternion Transform::GetRotation() const {
	Decomposition dec = Decompose(m_transform);
	return dec.orientation;
}

Vec3 Transform::GetEulerRotation() const {
	Decomposition dec = Decompose(m_transform);
	return glm::degrees(glm::eulerAngles(dec.orientation));
}

Vec3 Transform::GetScale() const {
	Decomposition dec = Decompose(m_transform);
	return dec.scale;
}

Vec3 Transform::GetRight() const {
	Decomposition dec = Decompose(m_transform);
	return glm::rotate(dec.orientation, Vec3(1, 0, 0));
}

Vec3 Transform::GetUp() const {
	Decomposition dec = Decompose(m_transform);
	return glm::rotate(dec.orientation, Vec3(0, 1, 0));
}

Vec3 Transform::GetForward() const {
	Decomposition dec = Decompose(m_transform);
	return glm::rotate(dec.orientation, Vec3(0, 0, -1));
}

void Transform::Set(Mat4 transform) {
	m_transform = transform;
	m_inverseTransform = glm::inverse(transform);
}

void Transform::Set(const Transform& transform) {
	m_transform = transform.m_transform;
	m_inverseTransform = transform.m_inverseTransform;
}

void Transform::Set(const Vec3& position, const Quaternion& rotation, const Vec3& scale) {
	m_transform = Mat4(rotation);
	m_transform = glm::scale(m_transform, scale);
	m_transform = glm::translate(m_transform, position);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::LookAt(Vec3 pos, Vec3 look, Vec3 up) {
	m_inverseTransform = glm::lookAt(pos, look, up) ;
	m_transform = glm::inverse(m_inverseTransform);
}

void Transform::SetPosition(const Vec3& pos) {
	Decomposition dec = Decompose(m_transform);
	dec.translation = pos;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Translate(const Vec3& offset) {
	Decomposition dec = Decompose(m_transform);
	dec.translation += offset;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::SetRotation(const Quaternion& rotation) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation = rotation;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::SetEulerRotation(Vec3 degrees) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation = Quaternion(glm::radians(degrees));
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Rotate(const Quaternion& rotation) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation *= rotation;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Rotate(Float x, Float y, Float z) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation *= Quaternion({ x, y ,z });
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::RotateX(Float radians) {
	Rotate(radians, 0, 0);
}

void Transform::RotateY(Float radians) {
	Rotate(0, radians, 0);
}

void Transform::RotateZ(Float radians) {
	Rotate(0, 0, radians);
}

void Transform::RotateAroundAxis(Vec3 axis, Float radians) {
	Decomposition dec = Decompose(m_transform);
	dec.orientation = glm::rotate(dec.orientation, radians, axis);
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::SetScale(const Vec3& scale) {
	Decomposition dec = Decompose(m_transform);
	dec.scale = scale;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Scale(const Vec3& change) {
	Decomposition dec = Decompose(m_transform);
	dec.scale += change;
	m_transform = Recompose(dec);
	m_inverseTransform = glm::inverse(m_transform);
}

void Transform::Invert() {
	std::swap(m_transform, m_inverseTransform);
}

Vec3 Transform::ApplyPoint(Vec3 p) const {
	Vec4 transformed = m_transform * Vec4(p, 1.0);
	if (transformed.w == 1.0f) {
		return transformed;
	}
	else {
		return transformed / transformed.w;
	}
}

Vec3 Transform::ApplyVector(Vec3 v) const {
	return m_transform * Vec4(v, 0.0);
}

Vec3 Transform::ApplyNormal(Vec3 n) const {
	return Mat3(glm::transpose(glm::inverse(m_transform))) * n;
}

Ray Transform::ApplyRay(const Ray& r, Float* tMax) const {
	Vec3 o = ApplyPoint(r.origin);
	Vec3 d = ApplyVector(r.direction);

	Float lengthSquared = length2(d);
	if (lengthSquared > 0) {
		o += d * MachineEpsilon;
	}

	return Ray(o, d);
}

SurfaceInteraction Transform::ApplyInteraction(const SurfaceInteraction& in) const {
	SurfaceInteraction ret(in);
	ret.position = ApplyPoint(in.position);
	ret.normal = ApplyNormal(in.normal);
	if (length2(ret.normal) > 0.0f) {
		ret.normal = glm::normalize(ret.normal);
	}
	ret.wo = ApplyVector(in.wo);
	if (length2(ret.wo) > 0) {
		ret.wo = glm::normalize(ret.wo);
	}
	return ret;
}

Bounds3f Transform::ApplyBounds(const Bounds3f& b) const {
	Bounds3f bt;
	for (int32_t i = 0; i < 8; ++i) {
		bt = Union(bt, ApplyBounds(b));
	}
	return bt;
}

Vec3 Transform::ApplyInversePoint(Vec3 p) const {
	Vec4 transformed = m_inverseTransform * Vec4(p, 1.0);
	if (transformed.w == 1.0f) {
		return transformed;
	}
	else {
		return transformed / transformed.w;
	}
}

Vec3 Transform::ApplyInverseVector(Vec3 v) const {
	return m_inverseTransform * Vec4(v, 0.0);
}

Vec3 Transform::ApplyInverseNormal(Vec3 n) const {
	return Mat3(glm::transpose(glm::inverse(m_inverseTransform))) * n;
}

Ray Transform::ApplyInverseRay(const Ray& r, Float* tMax) const {
	Vec3 o = ApplyInversePoint(r.origin);
	Vec3 d = ApplyInverseVector(r.direction);

	Float lengthSquared = length2(d);
	if (lengthSquared > 0.0f) {
		o -= d * MachineEpsilon;
	}

	return Ray(o, d);
}

SurfaceInteraction Transform::ApplyInverseInteraction(const SurfaceInteraction& in) const {
	SurfaceInteraction ret(in);
	Transform t = Inverse(*this);
	ret.position = t.ApplyPoint(in.position);
	ret.normal = t.ApplyNormal(in.normal);
	if (length2(ret.normal) > 0.0f) {
		ret.normal = glm::normalize(ret.normal);
	}
	ret.wo = t.ApplyVector(in.wo);
	if (length2(ret.wo) > 0.0f) {
		ret.wo = glm::normalize(ret.wo);
	}
	return ret;
}

bool Transform::IsIdentity() const {
	return m_transform == Mat4(1.0f);
}

bool Transform::HasScale(Float tolerance) const {
	Float la2 = length2(ApplyVector(Vec3(1, 0, 0)));
	Float lb2 = length2(ApplyVector(Vec3(0, 1, 0)));
	Float lc2 = length2(ApplyVector(Vec3(0, 0, 1)));
	return (std::abs(la2 - 1) > tolerance || std::abs(lb2 - 1) > tolerance || std::abs(lc2 - 1) > tolerance);
}

bool Transform::SwapsHandedness() const {
	return glm::determinant(Mat3(m_transform)) < 0;
}

bool Transform::operator==(const Transform& t) const {
	return t.m_transform == m_transform;
}

bool Transform::operator!=(const Transform& t) const {
	return t.m_transform != m_transform;
}

Transform Transform::operator*(const Transform& t2) const {
	return Transform(m_transform * t2.m_transform, t2.m_inverseTransform * m_inverseTransform);
}

Transform Inverse(const Transform& t) {
	return Transform(t.GetInverseMatrix(), t.GetMatrix());
}

Transform Transpose(const Transform& t) {
	return Transform(glm::transpose(t.GetMatrix()), glm::transpose(t.GetInverseMatrix()));
}

Transform RotateFromTo(Vec3 from, Vec3 to) {
	Vec3 refl;
	if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f) {
		refl = Vec3(1, 0, 0);
	}
	else if (std::abs(from.y) < 0.72f && std::abs(to.y) < 0.72f) {
		refl = Vec3(0, 1, 0);
	}
	else {
		refl = Vec3(0, 0, 1);
	}

	Vec3 u = refl - from, v = refl - to;
	Mat4 r;
	for (int32_t i = 0; i < 3; ++i) {
		for (int32_t j = 0; j < 3; ++j) {
			r[i][j] = ((i == j) ? 1 : 0) - 2 / glm::dot(u, u) * u[i] * u[j] -
				2 / glm::dot(v, v) * v[i] * v[j] +
				4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];
		}
	}

	return Transform(r, glm::transpose(r));
}

Transform RotateAroundAxis(Float sinTheta, Float cosTheta, Vec3 axis) {
	Vec3 a = glm::normalize(axis);
	Mat4 m = Mat4(1.0f);
	m[0][0] = a.x * a.x + ((Float)1 - a.x * a.x) * cosTheta;
	m[1][0] = a.x * a.y * ((Float)1 - cosTheta) - a.z * sinTheta;
	m[2][0] = a.x * a.z * ((Float)1 - cosTheta) + a.y * sinTheta;
	m[3][0] = 0;

	m[0][1] = a.x * a.y * ((Float)1 - cosTheta) + a.z * sinTheta;
	m[1][1] = a.y * a.y + ((Float)1 - a.y * a.y) * cosTheta;
	m[2][1] = a.y * a.z * ((Float)1 - cosTheta) - a.x * sinTheta;
	m[3][1] = 0;

	m[0][2] = a.x * a.z * ((Float)1 - cosTheta) - a.y * sinTheta;
	m[1][2] = a.y * a.z * ((Float)1 - cosTheta) + a.x * sinTheta;
	m[2][2] = a.z * a.z + ((Float)1 - a.z * a.z) * cosTheta;
	m[3][2] = 0;

	return Transform(m);
}

Transform RotateAroundAxis(Float theta, Vec3 axis) {
	Float sinTheta = std::sin(glm::radians(theta));
	Float cosTheta = std::cos(glm::radians(theta));
	return RotateAroundAxis(sinTheta, cosTheta, axis);
}
