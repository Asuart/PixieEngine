#pragma once
#include "pch.h"
#include "RayTracing/Ray.h"
#include "MathBase.h"

struct Bounds2i {
	glm::ivec2 min = glm::ivec2(std::numeric_limits<int32_t>::max());
	glm::ivec2 max = glm::ivec2(std::numeric_limits<int32_t>::lowest());

	Bounds2i() = default;
	explicit Bounds2i(glm::ivec2 p);
	Bounds2i(glm::ivec2 p1, glm::ivec2 p2);

	glm::ivec2 Diagonal() const;
	glm::ivec2 Center() const;
	Float Area() const;
	int32_t MaxDimension() const;
	glm::ivec2 Lerp(Float p) const;
	glm::ivec2 Lerp(Vec2 p) const;
	void BoundingCircle(Vec2* center, Float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	glm::ivec2 operator[](int32_t i) const;
	glm::ivec2& operator[](int32_t i);
	bool operator==(const Bounds2i& b) const;
	bool operator!=(const Bounds2i& b) const;
};

struct Bounds2f {
	Vec2 min = Vec2(std::numeric_limits<Float>::max());
	Vec2 max = Vec2(std::numeric_limits<Float>::lowest());

	Bounds2f() = default;
	explicit Bounds2f(Vec2 p);
	Bounds2f(Vec2 p1, Vec2 p2);

	Vec2 Diagonal() const;
	Vec2 Center() const;
	Float Area() const;
	int32_t MaxDimension() const;
	Vec2 Lerp(Float p) const;
	Vec2 Lerp(Vec2 p) const;
	Vec2 Offset(Vec2 p) const;
	void BoundingCircle(Vec2* center, Float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	Vec2 operator[](int32_t i) const;
	Vec2& operator[](int32_t i);
	bool operator==(const Bounds2f& b) const;
	bool operator!=(const Bounds2f& b) const;
};

class Bounds3i {
public:
	glm::ivec3 min = glm::ivec3(std::numeric_limits<int32_t>::max());
	glm::ivec3 max = glm::ivec3(std::numeric_limits<int32_t>::lowest());

	Bounds3i() = default;
	explicit Bounds3i(glm::ivec3 p);
	Bounds3i(glm::ivec3 p1, glm::ivec3 p2);

	glm::ivec3 Corner(int32_t corner) const;
	glm::ivec3 Diagonal() const;
	glm::ivec3 Center() const;
	Float Area() const;
	Float Volume() const;
	int32_t MaxDimension() const;
	glm::ivec3 Lerp(Float t) const;
	glm::ivec3 Lerp(Vec3 t) const;
	glm::ivec3 Offset(glm::ivec3 p) const;
	void BoundingSphere(Vec3* center, Float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	glm::ivec3 operator[](int32_t i) const;
	glm::ivec3& operator[](int32_t i);
	bool operator==(const Bounds3i& b) const;
	bool operator!=(const Bounds3i& b) const;
};

class Bounds3f {
public:
	Vec3 min = Vec3(std::numeric_limits<Float>::max());
	Vec3 max = Vec3(std::numeric_limits<Float>::lowest());

	Bounds3f() = default;
	explicit Bounds3f(Vec3 p);
	Bounds3f(Vec3 p1, Vec3 p2);

	Vec3 Corner(int32_t corner) const;
	Vec3 Diagonal() const;
	Vec3 Center() const;
	Float Area() const;
	Float Volume() const;
	int32_t MaxDimension() const;
	Vec3 Lerp(Float t) const;
	Vec3 Lerp(Vec3 t) const;
	Vec3 Offset(Vec3 p) const;
	void BoundingSphere(Vec3* center, Float* radius) const;
	bool IsEmpty() const;
	bool IsDegenerate() const;

	Vec3 operator[](int32_t i) const;
	Vec3& operator[](int32_t i);
	bool operator==(const Bounds3f& b) const;
	bool operator!=(const Bounds3f& b) const;
};

Bounds2i Union(const Bounds2i& b1, const Bounds2i& b2);
Bounds2f Union(const Bounds2f& b1, const Bounds2f& b2);
Bounds3i Union(const Bounds3i& b1, const Bounds3i& b2);
Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2);
Bounds3f Union(const Bounds3f& b, Vec3 p);
bool Inside(Vec3 p, const Bounds3f& b);
