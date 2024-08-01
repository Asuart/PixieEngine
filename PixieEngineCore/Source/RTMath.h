#pragma once
#include "PixieEngineCoreHeaders.h"

#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
const double Pi = 3.14159265358979323846;
const double InvPi = 0.31830988618379067154;
const double Inv2Pi = 0.15915494309189533577;
const double Inv4Pi = 0.07957747154594766788;
const double PiOver2 = 1.57079632679489661923;
const double PiOver4 = 0.78539816339744830961;
const double Sqrt2 = 1.41421356237309504880;
const double MachineEpsilon = std::numeric_limits<double>::epsilon() * 0.5;
const double Infinity = std::numeric_limits<double>::infinity();
const double ShadowEpsilon = 0.00001;
const double MinusEpsilon = 0x1.fffffffffffffp-1;
#else
const Float Pi = 3.1415926535f;
const Float InvPi = 0.3183098861f;
const Float Inv2Pi = 0.1591549430f;
const Float Inv4Pi = 0.0795774715f;
const Float PiOver2 = 1.5707963267f;
const Float PiOver4 = 0.7853981633f;
const Float Sqrt2 = 1.4142135623f;
const Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5f;
const Float Infinity = std::numeric_limits<Float>::infinity();
const Float ShadowEpsilon = 0.0001f;
const Float MinusEpsilon = 0x1.fffffep-1;
#endif

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
