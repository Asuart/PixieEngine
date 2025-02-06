#pragma once
#include "Random.h"

class PerlinNoise {
public:
    PerlinNoise();
    ~PerlinNoise();

    Float Sample(const Vec3& p) const;
    Float Turbulence(const Vec3& p, int32_t depth = 7) const;

private:
    static const int32_t point_count = 256;
    Vec3* ranvec;
    int32_t* perm_x;
    int32_t* perm_y;
    int32_t* perm_z;

    static int32_t* PerlinGeneratePerm();
    static void Permute(int32_t* p, int32_t n);
    static Float TrilinearInterp(Vec3 c[2][2][2], Float u, Float v, Float w);
};
