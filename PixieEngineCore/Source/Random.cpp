#include "pch.h"
#include "Random.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0, 1);

inline Float RandomFloat() {
	return (Float)dist(gen);
}

inline Float RandomFloat(Float min, Float max) {
	return min + (max - min) * RandomFloat();
}

inline Vec3 RandomVector() {
	return Vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

inline Vec3 RandomVector(Float min, Float max) {
	return Vec3(RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
}

inline Vec3 RandomUnitVector() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

Vec3 RandomInUnitSphere() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

Vec3 RandomInHemisphere(const Vec3& normal) {
	Vec3 uSphere = RandomUnitVector();
	if (dot(uSphere, normal) > 0.0) // In the same hemisphere as the normal
		return uSphere;
	return -uSphere;
}

inline Vec3 RandomInUnitDisk() {
	RandomInUnitSphere();
	return glm::normalize(Vec3(RandomFloat(-1, 1), RandomFloat(-1, 1), 0));
}

Vec3 RandomCosineDirection() {
	Float r1 = RandomFloat();
	Float r2 = RandomFloat();
	Float z = sqrt(1 - r2);

	Float phi = 2 * Pi * r1;
	Float x = cos(phi) * sqrt(r2);
	Float y = sin(phi) * sqrt(r2);

	return Vec3(x, y, z);
}

uint64_t MixBits(uint64_t v) {
	v ^= (v >> 31);
	v *= 0x7fb5d329728ea185;
	v ^= (v >> 27);
	v *= 0x81dadef4bc2dd44d;
	v ^= (v >> 33);
	return v;
}

int32_t PermutationElement(uint32_t i, uint32_t l, uint32_t p) {
	uint32_t w = l - 1;
	w |= w >> 1;
	w |= w >> 2;
	w |= w >> 4;
	w |= w >> 8;
	w |= w >> 16;
	do {
		i ^= p;
		i *= 0xe170893d;
		i ^= p >> 16;
		i ^= (i & w) >> 4;
		i ^= p >> 8;
		i *= 0x0929eb3f;
		i ^= p >> 23;
		i ^= (i & w) >> 1;
		i *= 1 | p >> 27;
		i *= 0x6935fa69;
		i ^= (i & w) >> 11;
		i *= 0x74dcb303;
		i ^= (i & w) >> 2;
		i *= 0x9e501cc3;
		i ^= (i & w) >> 2;
		i *= 0xc860a3df;
		i &= w;
		i ^= i >> 5;
	} while (i >= l);
	return (i + p) % l;
}

RNG::RNG() 
	: m_state(PCG32_DEFAULT_STATE), m_inc(PCG32_DEFAULT_STREAM) {}

RNG::RNG(uint64_t seqIndex, uint64_t seed) { 
	SetSequence(seqIndex, seed); 
}

RNG::RNG(uint64_t seqIndex) { 
	SetSequence(seqIndex);
}

void RNG::SetSequence(uint64_t sequenceIndex, uint64_t seed) {
	m_state = 0u;
	m_inc = (sequenceIndex << 1u) | 1u;
	Uniform<uint32_t>();
	m_state += seed;
	Uniform<uint32_t>();
}

void RNG::SetSequence(uint64_t sequenceIndex) {
	SetSequence(sequenceIndex, MixBits(sequenceIndex));
}

void RNG::Advance(int64_t idelta) {
	uint64_t curMult = PCG32_MULT, curPlus = m_inc, accMult = 1u;
	uint64_t accPlus = 0u, delta = (uint64_t)idelta;
	while (delta > 0) {
		if (delta & 1) {
			accMult *= curMult;
			accPlus = accPlus * curMult + curPlus;
		}
		curPlus = (curMult + 1) * curPlus;
		curMult *= curMult;
		delta /= 2;
	}
	m_state = accMult * m_state + accPlus;
}
