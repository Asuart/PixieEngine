#pragma once
#include "pch.h"
#include "Math/Random.h"

template<uint32_t size>
struct SSAOKernel {
	std::array<Vec3, size> vectors;

	SSAOKernel() {
		GenerateValues();
	}

	void GenerateValues() {
		for (int32_t i = 0; i < size; i++) {
			Vec3 sample = glm::normalize(Vec3(RandomFloat() * 2.0f - 1.0f, RandomFloat() * 2.0f - 1.0f, RandomFloat())) * RandomFloat();
			Float scale = (Float)i / (Float)size;
			vectors[i] = sample * SSAOKernelLerp(0.1f, 1.0f, scale * scale);
		}
	}

	inline uint32_t Size() {
		return size;
	}

protected:
	inline Float SSAOKernelLerp(Float a, Float b, Float f) {
		return a + f * (b - a);
	}
};
