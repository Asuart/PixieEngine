#pragma once
#include "pch.h"
#include "RayTracing/Spectrum.h"
#include "ComplexNumber.h"

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
static constexpr Float MaxDegrees = 360.0f;
static constexpr Float MinSphericalSampleArea = 3e-4f;
static constexpr Float MaxSphericalSampleArea = 6.22f;
static constexpr Float UniformSpherePDF = Inv4Pi;
static constexpr Float UniformHemispherePDF = Inv2Pi;
static constexpr Float minFloat = std::numeric_limits<Float>::lowest();
static constexpr Float maxFloat = std::numeric_limits<Float>::max();
static constexpr Float FloatOneMinusEpsilon = float(0x1.fffffep-1);
static constexpr Float DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1f;
#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
static constexpr Float OneMinusEpsilon = DoubleOneMinusEpsilon;
#else 
static constexpr Float OneMinusEpsilon = FloatOneMinusEpsilon;
#endif

inline bool isnan(const Vec2& v) {
	return std::isnan(v.x) || std::isnan(v.y);
}

inline bool isnan(const Vec3& v) {
	return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
}

constexpr int32_t Lerp(Float t, int32_t from, int32_t to) {
	return int32_t(from + (to - from) * t);
}

constexpr uint32_t Lerp(Float t, uint32_t from, uint32_t to) {
	return uint32_t(from + (to - from) * t);
}

constexpr Float Lerp(Float t, Float from, Float to) {
	return Float(from + (to - from) * t);
}

constexpr Vec2 Lerp(Float t, Vec2 from, Vec2 to) {
	return from + (to - from) * t;
}

constexpr Vec3 Lerp(Float t, Vec3 from, Vec3 to) {
	return from + (to - from) * t;
}

constexpr glm::ivec2 Lerp(Float t, glm::ivec2 from, glm::ivec2 to) {
	return glm::ivec2(Vec2(from) + Vec2(to - from) * t);
}

constexpr glm::ivec3 Lerp(Float t, glm::ivec3 from, glm::ivec3 to) {
	return glm::ivec3(Vec3(from) + Vec3(to - from) * t);
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

inline int32_t MaxComponentIndex(const Vec3& d) {
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
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

inline Vec3 SphericalDirection(Float sinTheta, Float cosTheta, Float phi) {
	return Vec3(Clamp(sinTheta, -1, 1) * std::cos(phi), Clamp(sinTheta, -1, 1) * std::sin(phi), Clamp(cosTheta, -1, 1));
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

template <typename Float, typename C>
inline constexpr Float EvaluatePolynomial(Float t, C c) {
	return c;
}

template <typename Float, typename C, typename... Args>
inline constexpr Float EvaluatePolynomial(Float t, C c, Args... cRemaining) {
	return std::fma(t, EvaluatePolynomial(t, cRemaining...), c);
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

inline float FastExp(float x) {
	float xp = x * 1.442695041f;
	float fxp = std::floor(xp), f = xp - fxp;
	int32_t i = (int32_t)fxp;
	float twoToF = EvaluatePolynomial(f, 1.f, 0.695556856f, 0.226173572f, 0.0781455737f);
	int32_t exponent = Exponent(twoToF) + i;
	if (exponent < -126) {
		return 0;
	}
	if (exponent > 127) {
		return Infinity;
	}
	uint32_t bits = FloatToBits(twoToF);
	bits &= 0b10000000011111111111111111111111u;
	bits |= (exponent + 127) << 23;
	return BitsToFloat(bits);
}

inline Float Gaussian(Float x, Float mu = 0, Float sigma = 1) {
	return (Float)1 / std::sqrt((Float)2 * Pi * sigma * sigma) * (Float)FastExp((float)(-Sqr(x - mu) / ((Float)2 * sigma * sigma)));
}

inline Float GaussianIntegral(Float x0, Float x1, Float mu = 0, Float sigma = 1) {
	Float sigmaRoot2 = sigma * Float(1.414213562373095f);
	return 0.5f * (std::erf((mu - x0) / sigmaRoot2) - std::erf((mu - x1) / sigmaRoot2));
}

template <typename Predicate>
inline size_t FindInterval(size_t sz, const Predicate& pred) {
	using ssize_t = std::make_signed_t<size_t>;
	ssize_t size = (ssize_t)sz - 2, first = 1;
	while (size > 0) {
		size_t half = (size_t)size >> 1, middle = first + half;
		bool predResult = pred((int32_t)middle);
		first = predResult ? middle + 1 : first;
		size = predResult ? size - (half + 1) : half;
	}
	return (size_t)Clamp((size_t)first - 1, 0, sz - 2);
}


inline Vec3 FaceForward(Vec3  n, Vec3  v) {
	return (glm::dot(n, v) < 0.) ? -n : n;
}

inline Vec3 Reflect(Vec3 wo, Vec3 n) {
	return -wo + 2 * glm::dot(wo, n) * n;
}

inline bool Refract(Vec3 wi, Vec3 n, Float eta, Float* etap, Vec3* wt) {
	Float cosTheta_i = glm::dot(n, wi);

	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
		n = -n;
	}

	Float sin2Theta_i = std::max<Float>(0, 1 - Sqr(cosTheta_i));
	Float sin2Theta_t = sin2Theta_i / Sqr(eta);

	if (sin2Theta_t >= 1) {
		return false;
	}

	Float cosTheta_t = std::sqrt(1 - sin2Theta_t);

	*wt = -wi / eta + (cosTheta_i / eta - cosTheta_t) * Vec3(n);
	if (etap) {
		*etap = eta;
	}

	return true;
}

inline Vec2 SampleUniformDiskConcentric(Vec2 u) {
	Vec2 uOffset = (Float)2.0f * u - Vec2(1, 1);
	if (uOffset.x == 0 && uOffset.y == 0) {
		return { 0, 0 };
	}

	Float theta, r;
	if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
		r = uOffset.x;
		theta = PiOver4 * (uOffset.y / uOffset.x);
	}
	else {
		r = uOffset.y;
		theta = PiOver2 - PiOver4 * (uOffset.x / uOffset.y);
	}

	return r * Vec2(std::cos(theta), std::sin(theta));
}

inline Vec2 SampleUniformDiskPolar(Vec2 u) {
	Float r = std::sqrt(u[0]);
	Float theta = 2 * Pi * u[1];
	return { r * std::cos(theta), r * std::sin(theta) };
}

inline Vec3 SampleUniformHemisphere(Vec2 u) {
	Float z = u[0];
	Float r = glm::sqrt(1 - Sqr(z));
	Float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline Vec3 SampleUniformSphere(Vec2 u) {
	Float z = 1 - 2 * u[0];
	Float r = SafeSqrt(1 - Sqr(z));
	Float phi = 2 * Pi * u[1];
	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline Vec3 SampleCosineHemisphere(Vec2 u) {
	Vec2 d = SampleUniformDiskConcentric(u);
	Float z = SafeSqrt(1 - Sqr(d.x) - Sqr(d.y));
	return Vec3(d.x, d.y, z);
}

inline Float CosineHemispherePDF(Float cosTheta) {
	return cosTheta * InvPi;
}

inline Float SphericalTriangleArea(Vec3 a, Vec3 b, Vec3 c) {
	return std::abs(2 * std::atan2(glm::dot(a, glm::cross(b, c)), 1 + glm::dot(a, b) + glm::dot(a, c) + glm::dot(b, c)));
}

inline Vec3 SampleUniformTriangle(Vec2 u) {
	Float b0, b1;
	if (u[0] < u[1]) {
		b0 = u[0] / 2;
		b1 = u[1] - b0;
	}
	else {
		b1 = u[1] / 2;
		b0 = u[0] - b1;
	}
	return { b0, b1, 1 - b0 - b1 };
}

inline std::array<Float, 3> SampleSphericalTriangle(const std::array<Vec3, 3>& v, Vec3 p, Vec2 u, Float* pdf) {
	if (pdf) *pdf = 0;
	Vec3 a = glm::normalize(v[0] - p), b = glm::normalize(v[1] - p), c = glm::normalize(v[2] - p);
	Vec3 n_ab = glm::cross(a, b), n_bc = glm::cross(b, c), n_ca = glm::cross(c, a);
	if (length2(n_ab) == 0 || length2(n_bc) == 0 || length2(n_ca) == 0)
		return {};
	n_ab = glm::normalize(n_ab);
	n_bc = glm::normalize(n_bc);
	n_ca = glm::normalize(n_ca);

	Float alpha = AngleBetween(n_ab, -n_ca);
	Float beta = AngleBetween(n_bc, -n_ab);
	Float gamma = AngleBetween(n_ca, -n_bc);

	Float A_pi = alpha + beta + gamma;
	Float Ap_pi = Lerp(u[0], Pi, A_pi);
	if (pdf) {
		Float A = A_pi - Pi;
		*pdf = (A <= 0) ? 0 : 1 / A;
	}

	Float cosAlpha = std::cos(alpha), sinAlpha = std::sin(alpha);
	Float sinPhi = std::sin(Ap_pi) * cosAlpha - std::cos(Ap_pi) * sinAlpha;
	Float cosPhi = std::cos(Ap_pi) * cosAlpha + std::sin(Ap_pi) * sinAlpha;
	Float k1 = cosPhi + cosAlpha;
	Float k2 = sinPhi - sinAlpha * glm::dot(a, b) /* cos c */;
	Float cosBp = (k2 + (DifferenceOfProducts(k2, cosPhi, k1, sinPhi)) * cosAlpha) / ((SumOfProducts(k2, sinPhi, k1, cosPhi)) * sinAlpha);
	cosBp = Clamp(cosBp, -1, 1);

	Float sinBp = SafeSqrt(1 - Sqr(cosBp));
	Vec3 cp = cosBp * a + sinBp * glm::normalize(GramSchmidt(c, a));

	Float cosTheta = 1 - u[1] * (1 - glm::dot(cp, b));
	Float sinTheta = SafeSqrt(1 - Sqr(cosTheta));
	Vec3 w = cosTheta * b + sinTheta * glm::normalize(GramSchmidt(cp, b));
	Vec3 e1 = v[1] - v[0], e2 = v[2] - v[0];
	Vec3 s1 = glm::cross(w, e2);
	Float divisor = glm::dot(s1, e1);
	if (divisor == 0) {
		return { 1.f / 3.f, 1.f / 3.f, 1.f / 3.f };
	}
	Float invDivisor = 1 / divisor;
	Vec3 s = p - v[0];
	Float b1 = glm::dot(s, s1) * invDivisor;
	Float b2 = glm::dot(w, glm::cross(s, e1)) * invDivisor;

	b1 = Clamp(b1, 0, 1);
	b2 = Clamp(b2, 0, 1);
	if (b1 + b2 > 1) {
		b1 /= b1 + b2;
		b2 /= b1 + b2;
	}
	return { Float(1 - b1 - b2), Float(b1), Float(b2) };
}

inline Vec2 InvertSphericalTriangleSample(const std::array<Vec3, 3>& v, Vec3 p, Vec3 w) {
	Vec3 a = glm::normalize(v[0] - p), b = glm::normalize(v[1] - p), c = glm::normalize(v[2] - p);
	Vec3 n_ab = glm::cross(a, b), n_bc = glm::cross(b, c), n_ca = glm::cross(c, a);
	if (length2(n_ab) == 0 || length2(n_bc) == 0 || length2(n_ca) == 0) {
		return Vec2();
	}
	n_ab = glm::normalize(n_ab);
	n_bc = glm::normalize(n_bc);
	n_ca = glm::normalize(n_ca);

	Float alpha = AngleBetween(n_ab, -n_ca);
	Float beta = AngleBetween(n_bc, -n_ab);
	Float gamma = AngleBetween(n_ca, -n_bc);

	Vec3 cp = glm::normalize(glm::cross(glm::cross(b, w), glm::cross(c, a)));
	if (glm::dot(cp, a + c) < 0) {
		cp = -cp;
	}

	Float u0;
	if (glm::dot(a, cp) > 0.99999847691f) {
		u0 = 0;
	}
	else {
		Vec3 n_cpb = glm::cross(cp, b), n_acp = glm::cross(a, cp);
		if (length2(n_cpb) == 0 || length2(n_acp) == 0) {
			return Vec2(0.5, 0.5);
		}
		n_cpb = glm::normalize(n_cpb);
		n_acp = glm::normalize(n_acp);
		Float Ap = alpha + AngleBetween(n_ab, n_cpb) + AngleBetween(n_acp, -n_cpb) - Pi;

		Float A = alpha + beta + gamma - Pi;
		u0 = Ap / A;
	}

	Float u1 = (1 - glm::dot(w, b)) / (1 - glm::dot(cp, b));
	return Vec2(Clamp(u0, 0, 1), Clamp(u1, 0, 1));
}

inline Float FrDielectric(Float cosTheta_i, Float eta) {
	cosTheta_i = Clamp(cosTheta_i, -1, 1);

	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
	}

	Float sin2Theta_i = 1 - Sqr(cosTheta_i);
	Float sin2Theta_t = sin2Theta_i / Sqr(eta);
	if (sin2Theta_t >= 1) {
		return 1.0f;
	}
	Float cosTheta_t = glm::sqrt(1 - sin2Theta_t);

	Float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
	Float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
	return (Sqr(r_parl) + Sqr(r_perp)) / 2;
}

inline Float FrComplex(Float cosTheta_i, Complex<Float> eta) {
	cosTheta_i = Clamp(cosTheta_i, 0.0f, 1.0f);
	Float sin2Theta_i = 1.0f - Sqr(cosTheta_i);
	Complex<Float> sin2Theta_t = sin2Theta_i / Sqr(eta);
	Complex<Float> cosTheta_t = sqrt(1.0f - sin2Theta_t);

	Complex<Float> r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
	Complex<Float> r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
	return (norm(r_parl) + norm(r_perp)) / 2.0f;
}

inline Spectrum FrComplex(Float cosTheta_i, Spectrum eta, Spectrum k) {
	return Spectrum(
		FrComplex(cosTheta_i, Complex<Float>(eta.GetRGB().x, k.GetRGB().x)),
		FrComplex(cosTheta_i, Complex<Float>(eta.GetRGB().y, k.GetRGB().y)),
		FrComplex(cosTheta_i, Complex<Float>(eta.GetRGB().z, k.GetRGB().z))
	);
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

inline int32_t SampleDiscrete(std::vector<Float>& weights, Float u, Float* pmf = nullptr, Float* uRemapped = nullptr) {
	if (weights.empty()) {
		if (pmf) {
			*pmf = 0;
		}
		return -1;
	}

	Float sumWeights = 0;
	for (Float w : weights) {
		sumWeights += w;
	}

	Float up = u * sumWeights;
	if (up == sumWeights) {
		up = NextFloatDown(up);
	}

	int32_t offset = 0;
	Float sum = 0;
	while (sum + weights[offset] <= up) {
		sum += weights[offset++];
	}

	if (pmf) {
		*pmf = weights[offset] / sumWeights;
	}
	if (uRemapped) {
		*uRemapped = std::min((up - sum) / weights[offset], OneMinusEpsilon);
	}

	return offset;
}

inline float Aspect(glm::ivec2 resolution) {
	return (float)resolution.x / resolution.y;
}

inline Float IntegrateEdge(Vec3 v1, Vec3 v2, Vec3 N) {
	Float x = dot(v1, v2);
	Float y = abs(x);
	Float a = 0.8543985f + (0.4965155f + 0.0145206f * y) * y;
	Float b = 3.4175940f + (4.1616724f + y) * y;
	Float v = a / b;
	Float theta_sintheta = (x > 0.0f) ? v : 0.5f * (1.0f / sqrt(glm::max(1.0f - x * x, 1e-7f))) - v;
	return glm::dot(glm::cross(v1, v2) * theta_sintheta, N);
}

inline Vec3 EqualAreaSquareToSphere(Vec2 p) {
	Float u = 2 * p.x - 1, v = 2 * p.y - 1;
	Float up = std::abs(u), vp = std::abs(v);

	Float signedDistance = 1 - (up + vp);
	Float d = std::abs(signedDistance);
	Float r = 1 - d;

	Float phi = (r == 0 ? 1 : (vp - up) / r + 1) * Pi / 4;

	Float z = std::copysign(1 - Sqr(r), signedDistance);

	Float cosPhi = std::copysign(std::cos(phi), u);
	Float sinPhi = std::copysign(std::sin(phi), v);
	return Vec3(cosPhi * r * SafeSqrt(2 - Sqr(r)), sinPhi * r * SafeSqrt(2 - Sqr(r)), z);
}

// Via source code from Clarberg: Fast Equal-Area Mapping of the (Hemi)Sphere using SIMD
inline Vec2 EqualAreaSphereToSquare(Vec3 d) {
	Float x = std::abs(d.x), y = std::abs(d.y), z = std::abs(d.z);

	Float r = SafeSqrt(1 - z);

	Float a = std::max(x, y), b = std::min(x, y);
	b = a == 0 ? 0 : b / a;

	const Float t1 = 0.406758566246788489601959989e-5f;
	const Float t2 = 0.636226545274016134946890922156f;
	const Float t3 = 0.61572017898280213493197203466e-2f;
	const Float t4 = -0.247333733281268944196501420480f;
	const Float t5 = 0.881770664775316294736387951347e-1f;
	const Float t6 = 0.419038818029165735901852432784e-1f;
	const Float t7 = -0.251390972343483509333252996350e-1f;
	Float phi = EvaluatePolynomial(b, t1, t2, t3, t4, t5, t6, t7);

	if (x < y) {
		phi = 1 - phi;
	}

	Float v = phi * r;
	Float u = r - v;

	if (d.z < 0) {
		std::swap(u, v);
		u = 1 - u;
		v = 1 - v;
	}

	u = std::copysign(u, d.x);
	v = std::copysign(v, d.y);

	return Vec2(0.5f * (u + 1), 0.5f * (v + 1));
}
