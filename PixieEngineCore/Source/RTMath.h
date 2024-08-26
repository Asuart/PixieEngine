#pragma once
#include "pch.h"

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
static constexpr Float ShadowEpsilon = 0.00001f;
static constexpr Float OneMinusEpsilon = 0x1.fffffffffffffp-1f;
static constexpr Float MaxDegrees = 360.0f;
static constexpr Float MinSphericalSampleArea = 3e-4f;
static constexpr Float MaxSphericalSampleArea = 6.22f;
static constexpr Float UniformSpherePDF = Inv4Pi;
static constexpr Float UniformHemispherePDF = Inv2Pi;

Float SafeSqrt(Float v);
Float PowerHeuristic(int32_t nf, Float fPdf, int32_t ng, Float gPdf);
Float SphericalTriangleArea(Vec3 v1, Vec3 v2, Vec3 v3);
Float MaxComponent(const Vec3& v);
Float CosineHemispherePDF(Float cosTheta);
Float CosTheta(Vec3 w);
Float Cos2Theta(Vec3 w);
Float AbsCosTheta(Vec3 w);
Float Sin2Theta(Vec3 w);
Float SinTheta(Vec3 w);
Float TanTheta(Vec3 w);
Float Tan2Theta(Vec3 w);
Float CosPhi(Vec3 w);
Float SinPhi(Vec3 w);
Float length2(const Vec2& v);
Float length2(const Vec3& v);
Float gamma(int32_t n);
Float FrDielectric(Float cosTheta_i, Float eta);
Float AbsDot(const Vec3& v1, const Vec3& v2);
Float SmoothStep(Float x, Float a, Float b);

Vec2 SampleUniformDiskConcentric(Vec2 u);
Vec2 SampleUniformDiskPolar(Vec2 u);

Vec3 SampleUniformSphere(Vec2 u);
Vec3 SampleUniformHemisphere(Vec2 u);
Vec3 SampleCosineHemisphere(Vec2 u);
Vec3 FaceForward(Vec3  n, Vec3  v);
Vec3 Reflect(Vec3 wo, Vec3 n);

bool SameHemisphere(Vec3 w, Vec3 wp);
bool Refract(Vec3 wi, Vec3 n, Float eta, Float* etap, Vec3* wt);
bool isnan(const Vec3& v);

void CoordinateSystem(Vec3 v1, Vec3* v2, Vec3* v3);

Float DifferenceOfProducts(Float a, Float b, Float c, Float d);
Float SumOfProducts(Float a, Float b, Float c, Float d);
Float SafeASin(Float x);
Float AngleBetween(Vec3 v1, Vec3 v2);
Float SampleLinear(Float u, Float a, Float b);
Vec2 SampleBilinear(Vec2 u, const std::array<Float, 4>& w);
Float BilinearPDF(Vec2 p, const std::array<Float, 4>& w);
Vec3 GramSchmidt(Vec3 v, Vec3 w);
Vec3 SampleUniformTriangle(Vec2 u);
std::array<Float, 3> SampleSphericalTriangle(const std::array<Vec3, 3>& v, Vec3 p, Vec2 u, Float* pdf);
Vec2 InvertSphericalTriangleSample(const std::array<Vec3, 3>& v, Vec3 p, Vec3 w);

template<typename T>
T Lerp(Float t, T from, T to) {
	return from + (to - from) * t;
}

template<typename T>
T Sqr(T v) {
	return v * v;
}

template <typename T, typename U, typename V>
constexpr T Clamp(T val, U low, V high) {
	if (val < low)       return T(low);
	else if (val > high) return T(high);
	else                 return val;
}
