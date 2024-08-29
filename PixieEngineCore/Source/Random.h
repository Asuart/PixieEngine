#pragma once
#include "pch.h"
#include "RTMath.h"

Float RandomFloat();
Float RandomFloat(Float min, Float max);

Vec3 RandomVector();
Vec3 RandomVector(Float min, Float max);
Vec3 RandomUnitVector();

Vec3 RandomInUnitSphere();
Vec3 RandomInHemisphere(const Vec3& normal);
Vec3 RandomInUnitDisk();
Vec3 RandomCosineDirection();

uint64_t MixBits(uint64_t v);
int32_t PermutationElement(uint32_t i, uint32_t l, uint32_t p);

class RNG {
	static constexpr uint64_t PCG32_DEFAULT_STATE = 0x853c49e6748fea9bULL;
	static constexpr uint64_t PCG32_DEFAULT_STREAM = 0xda3e39cb94b95bdbULL;
	static constexpr uint64_t PCG32_MULT = 0x5851f42d4c957f2dULL;

public:
	RNG();
	RNG(uint64_t seqIndex, uint64_t seed);
	RNG(uint64_t seqIndex);

	void SetSequence(uint64_t sequenceIndex, uint64_t seed);
	void SetSequence(uint64_t sequenceIndex);
	void Advance(int64_t idelta);

	template <typename T>
	T Uniform();

	template <typename T>
	typename std::enable_if_t<std::is_integral_v<T>, T> Uniform(T b) {
		T threshold = (~b + 1u) % b;
		while (true) {
			T r = Uniform<T>();
			if (r >= threshold)
				return r % b;
		}
	}

protected:
	uint64_t m_state, m_inc;
};

template <>
inline uint32_t RNG::Uniform<uint32_t>() {
	uint64_t oldstate = m_state;
	m_state = oldstate * PCG32_MULT + m_inc;
	uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
	uint32_t rot = (uint32_t)(oldstate >> 59u);
	return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}

template <>
inline uint64_t RNG::Uniform<uint64_t>() {
	uint64_t v0 = Uniform<uint32_t>(), v1 = Uniform<uint32_t>();
	return (v0 << 32) | v1;
}

template <>
inline int32_t RNG::Uniform<int32_t>() {
	uint32_t v = Uniform<uint32_t>();
	if (v <= (uint32_t)std::numeric_limits<int32_t>::max())
		return int32_t(v);
	return int32_t(v - std::numeric_limits<int32_t>::min()) + std::numeric_limits<int32_t>::min();
}

template <>
inline int64_t RNG::Uniform<int64_t>() {
	uint64_t v = Uniform<uint64_t>();
	if (v <= (uint64_t)std::numeric_limits<int64_t>::max())
		return int64_t(v);
	return int64_t(v - std::numeric_limits<int64_t>::min()) +
		std::numeric_limits<int64_t>::min();
}

template <>
inline float RNG::Uniform<float>() {
	return std::min<float>(OneMinusEpsilon, Uniform<uint32_t>() * 0x1p-32f);
}

template <>
inline double RNG::Uniform<double>() {
	return std::min<double>(OneMinusEpsilon, Uniform<uint64_t>() * 0x1p-64);
}
