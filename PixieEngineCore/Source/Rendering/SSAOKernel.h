#pragma once
#include "pch.h"
#include "Math/Random.h"

template<uint32_t size>
struct SSAOKernel {
	std::array<glm::vec3, size> vectors;

	SSAOKernel() {
		GenerateValues();
	}

	void GenerateValues() {
		for (int32_t i = 0; i < size; i++) {
			glm::vec3 sample = glm::normalize(glm::vec3(RandomFloat() * 2.0f - 1.0f, RandomFloat() * 2.0f - 1.0f, RandomFloat())) * RandomFloat();
			float scale = (float)i / (float)size;
			vectors[i] = sample * SSAOKernelLerp(0.1f, 1.0f, scale * scale);
		}
	}

	inline uint32_t Size() const {
		return size;
	}

protected:
	inline float SSAOKernelLerp(float a, float b, float f) const {
		return a + f * (b - a);
	}
};
