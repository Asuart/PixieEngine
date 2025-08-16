#pragma once
#include "pch.h"

namespace PixieEngine {

static constexpr float Pi = 3.14159265358979323846f;
static constexpr float TwoPi = Pi * 2.0f;
static constexpr float InvPi = 0.31830988618379067154f;
static constexpr float Inv2Pi = 0.15915494309189533577f;
static constexpr float Inv4Pi = 0.07957747154594766788f;
static constexpr float PiOver2 = 1.57079632679489661923f;
static constexpr float PiOver4 = 0.78539816339744830961f;
static constexpr float Sqrt2 = 1.41421356237309504880f;
static constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;
static constexpr float Infinity = std::numeric_limits<float>::infinity();
static constexpr float ShadowEpsilon = 0.0001f;
static constexpr float MaxDegrees = 360.0f;
static constexpr float MinSphericalSampleArea = 3e-4f;
static constexpr float MaxSphericalSampleArea = 6.22f;
static constexpr float UniformSpherePDF = Inv4Pi;
static constexpr float UniformHemispherePDF = Inv2Pi;
static constexpr float minFloat = std::numeric_limits<float>::lowest();
static constexpr float maxFloat = std::numeric_limits<float>::max();
static constexpr float FloatOneMinusEpsilon = float(0x1.fffffep-1);
static constexpr float DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1f;
static constexpr float OneMinusEpsilon = FloatOneMinusEpsilon;

inline bool isnan(const glm::vec2& v) {
	return std::isnan(v.x) || std::isnan(v.y);
}

inline bool isnan(const glm::vec3& v) {
	return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}

constexpr int32_t Lerp(float t, int32_t from, int32_t to) {
	return int32_t(from + (to - from) * t);
}

constexpr uint32_t Lerp(float t, uint32_t from, uint32_t to) {
	return uint32_t(from + (to - from) * t);
}

constexpr float Lerp(float t, float from, float to) {
	return float(from + (to - from) * t);
}

constexpr glm::vec2 Lerp(float t, glm::vec2 from, glm::vec2 to) {
	return from + (to - from) * t;
}

constexpr glm::vec3 Lerp(float t, glm::vec3 from, glm::vec3 to) {
	return from + (to - from) * t;
}

constexpr glm::ivec2 Lerp(float t, glm::ivec2 from, glm::ivec2 to) {
	return glm::ivec2(glm::vec2(from) + glm::vec2(to - from) * t);
}

constexpr glm::ivec3 Lerp(float t, glm::ivec3 from, glm::ivec3 to) {
	return glm::ivec3(glm::vec3(from) + glm::vec3(to - from) * t);
}

template<typename T>
constexpr T Sqr(T v) {
	return v * v;
}

template <typename T, typename U, typename V>
constexpr T Clamp(T val, U low, V high) {
	if (val < low)       return T(low);
	else if (val > high) return T(high);
	else                 return val;
}

inline float MaxComponent(const glm::vec3& v) {
	return glm::max(v.x, glm::max(v.y, v.z));
}

inline int32_t MaxComponentIndex(const glm::vec3& d) {
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}

inline float SafeSqrt(float v) {
	return glm::sqrt(glm::max((float)0.0f, v));
}

inline float AbsDot(const glm::vec3& v1, const glm::vec3& v2) {
	return glm::abs(glm::dot(v1, v2));
}

inline float length2(const glm::vec2& v) {
	return v.x * v.x + v.y * v.y;
}

inline float length2(const glm::vec3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline void CoordinateSystem(glm::vec3 v1, glm::vec3* v2, glm::vec3* v3) {
	float sign = std::copysign(1.0f, v1.z);
	float a = -1 / (sign + v1.z);
	float b = v1.x * v1.y * a;
	*v2 = glm::vec3(1.0f + sign * Sqr(v1.x) * a, sign * b, -sign * v1.x);
	*v3 = glm::vec3(b, sign + Sqr(v1.y) * a, -v1.y);
}

inline uint32_t FloatToBits(float f) {
	return std::bit_cast<uint32_t>(f);
}

inline float BitsToFloat(uint32_t ui) {
	return std::bit_cast<float>(ui);
}

inline int32_t Exponent(float v) {
	return (FloatToBits(v) >> 23) - 127;
}

inline float NextFloatDown(float v) {
	if (std::isinf(v) && v < 0.0f) {
		return v;
	}
	if (v == 0.0f) {
		v = -0.0f;
	}
	uint32_t ui = FloatToBits(v);
	if (v > 0) {
		--ui;
	}
	else {
		++ui;
	}
	return BitsToFloat(ui);
}

inline float Aspect(glm::ivec2 resolution) {
	return (float)resolution.x / resolution.y;
}

inline float IntegrateEdge(glm::vec3 v1, glm::vec3 v2, glm::vec3 N) {
	float x = glm::dot(v1, v2);
	float y = abs(x);
	float a = 0.8543985f + (0.4965155f + 0.0145206f * y) * y;
	float b = 3.4175940f + (4.1616724f + y) * y;
	float v = a / b;
	float theta_sintheta = (x > 0.0f) ? v : 0.5f * (1.0f / sqrt(glm::max(1.0f - x * x, 1e-7f))) - v;
	return glm::dot(glm::cross(v1, v2) * theta_sintheta, N);
}

}