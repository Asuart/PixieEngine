#ifndef PIXIE_ENGINE_CORE_MATH
#define PIXIE_ENGINE_CORE_MATH

#include "pch.h"

const float Pi = 3.14159265358979323846;
const float InvPi = 0.31830988618379067154;
const float Inv2Pi = 0.15915494309189533577;
const float Inv4Pi = 0.07957747154594766788;
const float PiOver2 = 1.57079632679489661923;
const float PiOver4 = 0.78539816339744830961;
const float Sqrt2 = 1.41421356237309504880;

const float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;
const float ShadowEpsilon = 0.0001f;
const float Infinity = std::numeric_limits<float>::infinity();

const double DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1;
const float FloatOneMinusEpsilon = 0x1.fffffep-1;
const float OneMinusEpsilon = FloatOneMinusEpsilon;

template<typename T>
T Lerp(float t, T from, T to) {
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

float SafeSqrt(float v);
float PowerHeuristic(int nf, float fPdf, int ng, float gPdf);
float SphericalTriangleArea(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
float MaxComponent(const glm::vec3& v);
glm::vec3 SampleUniformSphere(glm::vec2 u);
float UniformSpherePDF();
glm::vec3 SampleUniformHemisphere(glm::vec2 u);
float UniformHemispherePDF();
void CoordinateSystem(glm::vec3 v1, glm::vec3* v2, glm::vec3* v3);
glm::vec2 SampleUniformDiskConcentric(glm::vec2 u);
glm::vec2 SampleUniformDiskPolar(glm::vec2 u);
bool SameHemisphere(glm::vec3 w, glm::vec3 wp);
glm::vec3 SampleCosineHemisphere(glm::vec2 u);
float CosineHemispherePDF(float cosTheta);
float CosTheta(glm::vec3 w);
float Cos2Theta(glm::vec3 w);
float AbsCosTheta(glm::vec3 w);
float Sin2Theta(glm::vec3 w);
float SinTheta(glm::vec3 w);
float TanTheta(glm::vec3 w);
float Tan2Theta(glm::vec3 w);
float CosPhi(glm::vec3 w);
float SinPhi(glm::vec3 w);

bool Refract(glm::vec3 wi, glm::vec3 n, float eta, float* etap, glm::vec3* wt);
float FrDielectric(float cosTheta_i, float eta);
glm::vec3 FaceForward(glm::vec3  n, glm::vec3  v);
glm::vec3 Reflect(glm::vec3 wo, glm::vec3 n);

float gamma(int32_t n);

bool isnan(const glm::vec3& v);

#endif // PIXIE_ENGINE_CORE_MATH