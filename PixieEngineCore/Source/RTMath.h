#pragma once
#include "pch.h"
#include "Complex.h"

static constexpr Float Pi = 3.14159265358979323846f;
static constexpr Float TwoPi = Pi * 2.0f;
static constexpr Float InvPi = 0.31830988618379067154f;
static constexpr Float Inv2Pi = 0.15915494309189533577f;
static constexpr Float Inv4Pi = 0.07957747154594766788f;
static constexpr Float PiOver2 = 1.57079632679489661923f;
static constexpr Float PiOver4 = 0.78539816339744830961f;
static constexpr Float Sqrt2 = 1.41421356237309504880f;
static constexpr Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5f;
static constexpr Float Infinity = std::numeric_limits<Float>::infinity();
static constexpr Float ShadowEpsilon = 0.0001f;
static constexpr Float OneMinusEpsilon = 1 - MachineEpsilon;
static constexpr Float MaxDegrees = 360.0f;
static constexpr Float MinSphericalSampleArea = 3e-4f;
static constexpr Float MaxSphericalSampleArea = 6.22f;
static constexpr Float UniformSpherePDF = Inv4Pi;
static constexpr Float UniformHemispherePDF = Inv2Pi;

inline bool isnan(const Vec2& v) {
    return std::isnan(v.x) || std::isnan(v.y);
}

inline bool isnan(const Vec3& v) {
    return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}

template<typename T>
constexpr T Lerp(Float t, T from, T to) {
    return from + (to - from) * t;
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

inline Float SmoothStep(Float x, Float a, Float b) {
    if (a == b) return (x < a) ? 0.0f : 1.0f;
    Float t = Clamp((x - a) / (b - a), 0, 1);
    return t * t * (3 - 2 * t);
}

inline Float MaxComponent(const Vec3& v) {
    return glm::max(v.x, glm::max(v.y, v.z));
}

inline Float SafeSqrt(Float v) {
    return glm::sqrt(glm::max((Float)0.0f, v));
}

inline Float SafeASin(Float x) {
    return std::asin(Clamp(x, -1, 1));
}

inline Float SafeACos(Float x) {
    return std::acos(Clamp(x, -1, 1));
}

inline Float CosTheta(Vec3 w) {
    return w.z;
}

inline Float Cos2Theta(Vec3 w) {
    return Sqr(w.z);
}

inline Float AbsCosTheta(Vec3 w) {
    return std::abs(w.z);
}

inline Float Sin2Theta(Vec3 w) {
    return std::max<Float>(0, 1 - Cos2Theta(w));
}

inline Float SinTheta(Vec3 w) {
    return std::sqrt(Sin2Theta(w));
}

inline Float TanTheta(Vec3 w) {
    return SinTheta(w) / CosTheta(w);
}

inline Float Tan2Theta(Vec3 w) {
    return Sin2Theta(w) / Cos2Theta(w);
}

inline Float CosPhi(Vec3 w) {
    Float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
}

inline Float SinPhi(Vec3 w) {
    Float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
}

inline bool SameHemisphere(Vec3 w, Vec3 wp) {
    return w.z * wp.z > 0;
}

inline Float AbsDot(const Vec3& v1, const Vec3& v2) {
    return glm::abs(glm::dot(v1, v2));
}

inline Float length2(const Vec2& v) {
    return v.x * v.x + v.y * v.y;
}

inline Float length2(const Vec3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline Float gamma(int32_t n) {
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

inline Float PowerHeuristic(int32_t nf, Float fPdf, int32_t ng, Float gPdf) {
    Float f = nf * fPdf, g = ng * gPdf;
    if (std::isinf(Sqr(f))) {
        return 1;
    }
    return Sqr(f) / (Sqr(f) + Sqr(g));
}

inline Float SumOfProducts(Float a, Float b, Float c, Float d) {
    Float cd = c * d;
    Float sumOfProducts = std::fma(a, b, cd);
    Float error = std::fma(c, d, -cd);
    return sumOfProducts + error;
}

inline Float DifferenceOfProducts(Float a, Float b, Float c, Float d) {
    Float cd = c * d;
    Float differenceOfProducts = std::fma(a, b, -cd);
    Float error = std::fma(-c, d, cd);
    return differenceOfProducts + error;
}

inline Float AngleBetween(Vec3 v1, Vec3 v2) {
    if (glm::dot(v1, v2) < 0) {
        return Pi - 2 * SafeASin(glm::length(v1 + v2) / 2);
    }
    else {
        return 2 * SafeASin(glm::length(v2 - v1) / 2);
    }
}

inline void CoordinateSystem(Vec3 v1, Vec3* v2, Vec3* v3) {
    Float sign = std::copysign(Float(1), v1.z);
    Float a = -1 / (sign + v1.z);
    Float b = v1.x * v1.y * a;
    *v2 = Vec3(1 + sign * Sqr(v1.x) * a, sign * b, -sign * v1.x);
    *v3 = Vec3(b, sign + Sqr(v1.y) * a, -v1.y);
}

inline Float SampleLinear(Float u, Float a, Float b) {
    if (u == 0 && a == 0) {
        return 0;
    }
    Float x = u * (a + b) / (a + std::sqrt(Lerp(u, Sqr(a), Sqr(b))));
    return std::min(x, OneMinusEpsilon);
}

inline Vec2 SampleBilinear(Vec2 u, const std::array<Float, 4>& w) {
    Vec2 p;
    p.y = SampleLinear(u[1], w[0] + w[1], w[2] + w[3]);
    p.x = SampleLinear(u[0], Lerp(p.y, w[0], w[2]), Lerp(p.y, w[1], w[3]));
    return p;
}

inline Float BilinearPDF(Vec2 p, const std::array<Float, 4>& w) {
    if (p.x < 0 || p.x > 1 || p.y < 0 || p.y > 1) {
        return 0;
    }
    if (w[0] + w[1] + w[2] + w[3] == 0) {
        return 1;
    }
    return 4 * ((1 - p[0]) * (1 - p[1]) * w[0] + p[0] * (1 - p[1]) * w[1] + (1 - p[0]) * p[1] * w[2] + p[0] * p[1] * w[3]) / (w[0] + w[1] + w[2] + w[3]);
}

inline Vec3 GramSchmidt(Vec3 v, Vec3 w) {
    return v - glm::dot(v, w) * w;
}
