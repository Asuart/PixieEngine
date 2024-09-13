#include "pch.h"
#include "Transform.h"

Transform::Transform(const Vec3& position, const Vec3& rotation, const Vec3& scale)
	: m_position(position), m_rotation(rotation), m_scale(scale) {
	UpdateMatrices();
	UpdateDirections();
};

Transform::Transform(const Mat4& m)
	: m_transform(m), m_inverseTransform(glm::inverse(m)) {
	Decompose();
	UpdateMatrices();
	UpdateDirections();
}

Transform::Transform(const Mat4& m, const Mat4& mInv)
	: m_transform(m), m_inverseTransform(mInv) {
	Decompose();
	UpdateMatrices();
	UpdateDirections();
}

const Mat4& Transform::GetMatrix() const {
	return m_transform;
}

const Mat4& Transform::GetInverseMatrix() const {
	return m_inverseTransform;
}

Vec3& Transform::GetPosition() {
	return m_position;
}

Vec3& Transform::GetRotation() {
	return m_rotation;
}

Vec3& Transform::GetScale() {
	return m_scale;
}

const Vec3& Transform::GetPositionValue() const {
	return m_position;
}

const Vec3& Transform::GetRotationValue() const {
	return m_rotation;
}

const Vec3& Transform::GetScaleValue() const {
	return m_scale;
}

void Transform::LookAt(Vec3 pos, Vec3 look, Vec3 up) {
	m_transform = glm::lookAt(pos, look, up);
	m_inverseTransform = glm::inverse(m_transform);
	Decompose();
	SetPosition(pos);
	AddRotationY(180.0f);
	UpdateDirections();
}

void Transform::SetPosition(Float x, Float y, Float z) {
	m_position = Vec3(x, y, z);
	UpdateMatrices();
}

void Transform::SetPosition(const Vec3& pos) {
	m_position = pos;
	UpdateMatrices();
}

void Transform::Move(Float x, Float y, Float z) {
	m_position += Vec3(x, y, z);
	UpdateMatrices();
}

void Transform::Move(const Vec3& offset) {
	m_position += offset;
	UpdateMatrices();
}

void Transform::MoveForward(Float value) {
	m_position += m_forward * value;
	UpdateMatrices();
}

void Transform::MoveRight(Float value) {
	m_position += m_right * value;
	UpdateMatrices();
}

void Transform::MoveUp(Float value) {
	m_position += m_up * value;
	UpdateMatrices();
}

void Transform::SetRotation(Float x, Float y, Float z) {
	SetRotation(m_rotation + Vec3(x, y, z));
}

void Transform::SetRotation(const Vec3& rotation) {
	m_rotation = rotation;
	if (m_rotation.x >= MaxDegrees) m_rotation.x -= std::floor(m_rotation.x / MaxDegrees) * MaxDegrees;
	if (m_rotation.x <= -MaxDegrees) m_rotation.x += std::floor(-m_rotation.x / MaxDegrees) * MaxDegrees;
	if (m_rotation.y >= MaxDegrees) m_rotation.y -= std::floor(m_rotation.y / MaxDegrees) * MaxDegrees;
	if (m_rotation.y <= -MaxDegrees) m_rotation.y += std::floor(-m_rotation.y / MaxDegrees) * MaxDegrees;
	if (m_rotation.z >= MaxDegrees) m_rotation.z -= std::floor(m_rotation.z / MaxDegrees) * MaxDegrees;
	if (m_rotation.z <= -MaxDegrees) m_rotation.z += std::floor(-m_rotation.z / MaxDegrees) * MaxDegrees;
	UpdateMatrices();
	UpdateDirections();
}

void Transform::SetRotationX(Float rotation) {
	m_rotation.x = rotation;
	if (m_rotation.x >= MaxDegrees) m_rotation.x -= std::floor(m_rotation.x / MaxDegrees) * MaxDegrees;
	if (m_rotation.x <= -MaxDegrees) m_rotation.x += std::floor(-m_rotation.x / MaxDegrees) * MaxDegrees;
	UpdateMatrices();
	UpdateDirections();
}

void Transform::SetRotationY(Float rotation) {
	m_rotation.y = rotation;
	if (m_rotation.y >= MaxDegrees) m_rotation.y -= std::floor(m_rotation.y / MaxDegrees) * MaxDegrees;
	if (m_rotation.y <= -MaxDegrees) m_rotation.y += std::floor(-m_rotation.y / MaxDegrees) * MaxDegrees;
	UpdateMatrices();
	UpdateDirections();
}

void Transform::SetRotationZ(Float rotation) {
	m_rotation.z = rotation;
	if (m_rotation.z >= MaxDegrees) m_rotation.z -= std::floor(m_rotation.z / MaxDegrees) * MaxDegrees;
	if (m_rotation.z <= -MaxDegrees) m_rotation.z += std::floor(-m_rotation.z / MaxDegrees) * MaxDegrees;
	UpdateMatrices();
	UpdateDirections();
}

void Transform::AddRotation(Float x, Float y, Float z) {
	SetRotation(m_rotation + Vec3(x, y, z));
}

void Transform::AddRotationX(Float rotation) {
	SetRotationX(m_rotation.x + rotation);
}

void Transform::AddRotationY(Float rotation) {
	SetRotationY(m_rotation.y + rotation);
}

void Transform::AddRotationZ(Float rotation) {
	SetRotationX(m_rotation.z + rotation);
}

void Transform::SetScale(Float x, Float y, Float z) {
	m_scale = Vec3(x, y, z);
	UpdateMatrices();
}

void Transform::SetScale(const Vec3& scale) {
	m_scale = scale;
	UpdateMatrices();
}

void Transform::AddScale(Float x, Float y, Float z) {
	m_scale += Vec3(x, y, z);
	UpdateMatrices();
}

void Transform::AddScale(const Vec3& change) {
	m_scale += change;
	UpdateMatrices();
}

void Transform::UpdateMatrices() {
	Mat4 rotateX = glm::rotate(Mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	Mat4 rotateY = glm::rotate(Mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	Mat4 rotateZ = glm::rotate(Mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	Mat4 mRotate = rotateY * rotateX * rotateZ;
	Mat4 mTranslate = glm::translate(Mat4(1.0f), m_position);
	Mat4 mScale = glm::scale(Mat4(1.0f), m_scale);
	m_transform = mTranslate * mRotate * mScale;
	m_inverseTransform = glm::inverse(m_transform);
	UpdateDirections();
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

void Transform::Decompose() {
	glm::vec3 skew;
	glm::vec4 perspective;
	Quaternion qRotation;
	glm::decompose(m_transform, m_scale, qRotation, m_position, skew, perspective);
	m_rotation = glm::degrees(glm::eulerAngles(qRotation));
}

void Transform::UpdateDirections() {
	Mat4 rotateX = glm::rotate(Mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	Mat4 rotateY = glm::rotate(Mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	Mat4 rotateZ = glm::rotate(Mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	Mat4 mRotate = rotateY * rotateX * rotateZ;
	m_forward = glm::normalize(mRotate * glm::vec4(0.0, 0.0, -1.0, 0.0));
	m_up = glm::normalize(mRotate * glm::vec4(0.0, 1.0, 0.0, 0.0));
	m_right = glm::normalize(mRotate * glm::vec4(1.0, 0.0, 0.0, 0.0));
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

Transform Rotate(Float sinTheta, Float cosTheta, Vec3 axis) {
	Vec3 a = glm::normalize(axis);
	glm::mat4 m;
	m[0][0] = a.x * a.x + (1 - a.x * a.x) * cosTheta;
	m[0][1] = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
	m[0][2] = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
	m[0][3] = 0;

	m[1][0] = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
	m[1][1] = a.y * a.y + (1 - a.y * a.y) * cosTheta;
	m[1][2] = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
	m[1][3] = 0;

	m[2][0] = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
	m[2][1] = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
	m[2][2] = a.z * a.z + (1 - a.z * a.z) * cosTheta;
	m[2][3] = 0;

	return Transform(m);
}

Transform Rotate(Float theta, Vec3 axis) {
	Float sinTheta = std::sin(glm::radians(theta));
	Float cosTheta = std::cos(glm::radians(theta));
	return Rotate(sinTheta, cosTheta, axis);
}
