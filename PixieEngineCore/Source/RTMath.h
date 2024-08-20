#pragma once
#include "pch.h"

#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
const double Pi = 3.14159265358979323846;
const double TwoPi = Pi * 2.0;
const double InvPi = 0.31830988618379067154;
const double Inv2Pi = 0.15915494309189533577;
const double Inv4Pi = 0.07957747154594766788;
const double PiOver2 = 1.57079632679489661923;
const double PiOver4 = 0.78539816339744830961;
const double Sqrt2 = 1.41421356237309504880;
const double MachineEpsilon = std::numeric_limits<double>::epsilon() * 0.5;
const double Infinity = std::numeric_limits<double>::infinity();
const double ShadowEpsilon = 0.00001;
const double OneMinusEpsilon = 0x1.fffffffffffffp-1;
const double MaxDegrees = 360.0;
#else
const float Pi = 3.1415926535f;
const float TwoPi = Pi * 2.0f;
const float InvPi = 0.3183098861f;
const float Inv2Pi = 0.1591549430f;
const float Inv4Pi = 0.0795774715f;
const float PiOver2 = 1.5707963267f;
const float PiOver4 = 0.7853981633f;
const float Sqrt2 = 1.4142135623f;
const float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5f;
const float Infinity = std::numeric_limits<Float>::infinity();
const float ShadowEpsilon = 0.0001f;
const float OneMinusEpsilon = 0x1.fffffep-1;
const float MaxDegrees = 360.0f;
#endif

static constexpr Float MinSphericalSampleArea = 3e-4f;
static constexpr Float MaxSphericalSampleArea = 6.22f;

Float SafeSqrt(Float v);
Float PowerHeuristic(int32_t nf, Float fPdf, int32_t ng, Float gPdf);
Float SphericalTriangleArea(Vec3 v1, Vec3 v2, Vec3 v3);
Float MaxComponent(const Vec3& v);
Float UniformSpherePDF();
Float UniformHemispherePDF();
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
std::array<Float, 3> SampleSphericalTriangle(const std::array<Vec3, 3>& v, Vec3 p, Vec2 u, Float* pdf);

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
