#include "PerlinNoise.h"

PerlinNoise::PerlinNoise() {
	ranvec = new Vec3[point_count];
	for (int32_t i = 0; i < point_count; ++i) {
		ranvec[i] = glm::normalize(RandomVector(-1.0, 1.0));
	}

	perm_x = PerlinGeneratePerm();
	perm_y = PerlinGeneratePerm();
	perm_z = PerlinGeneratePerm();
}

PerlinNoise::~PerlinNoise() {
	delete[] ranvec;
	delete[] perm_x;
	delete[] perm_y;
	delete[] perm_z;
}

Float PerlinNoise::Sample(const Vec3& p) const {
	Float u = p.x - floor(p.x);
	Float v = p.y - floor(p.y);
	Float w = p.z - floor(p.z);

	u = u * u * (3 - 2 * u);
	v = v * v * (3 - 2 * v);
	w = w * w * (3 - 2 * w);

	int32_t i = static_cast<int32_t>(floor(p.x));
	int32_t j = static_cast<int32_t>(floor(p.y));
	int32_t k = static_cast<int32_t>(floor(p.z));
	Vec3 c[2][2][2];

	for (int32_t di = 0; di < 2; di++) {
		for (int32_t dj = 0; dj < 2; dj++) {
			for (int32_t dk = 0; dk < 2; dk++) {
				c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
			}
		}
	}

	return TrilinearInterp(c, u, v, w);
}

Float PerlinNoise::Turbulence(const Vec3& p, int32_t depth) const {
	Float accum = 0.0;
	Vec3 temp_p = p;
	Float weight = 1.0;

	for (int32_t i = 0; i < depth; i++) {
		accum += weight * Sample(temp_p);
		weight *= 0.5;
		temp_p *= 2;
	}

	return fabs(accum);
}

int32_t* PerlinNoise::PerlinGeneratePerm() {
	int32_t* p = new int32_t[point_count];

	for (int32_t i = 0; i < PerlinNoise::point_count; i++) {
		p[i] = i;
	}

	Permute(p, point_count);

	return p;
}

void PerlinNoise::Permute(int32_t* p, int32_t n) {
	for (int32_t i = n - 1; i > 0; i--) {
		int32_t target = rand() % i;
		int32_t tmp = p[i];
		p[i] = p[target];
		p[target] = tmp;
	}
}

Float PerlinNoise::TrilinearInterp(Vec3 c[2][2][2], Float u, Float v, Float w) {
	Float uu = u * u * (3 - 2 * u);
	Float vv = v * v * (3 - 2 * v);
	Float ww = w * w * (3 - 2 * w);
	Float accum = 0.0;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				Vec3 weight_v(u - i, v - j, w - k);
				accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
			}
		}
	}
	return accum;
}