#pragma once
#include "Random.h"

class PerlinNoise {
public:
    PerlinNoise();
    ~PerlinNoise();

    float Sample(const glm::vec3& p) const;
    float Turbulence(const glm::vec3& p, int32_t depth = 7) const;

private:
    static const int32_t point_count = 256;
    glm::vec3* ranvec;
    int32_t* perm_x;
    int32_t* perm_y;
    int32_t* perm_z;

    static int32_t* PerlinGeneratePerm();
    static void Permute(int32_t* p, int32_t n);
    static float TrilinearInterp(glm::vec3 c[2][2][2], float u, float v, float w);
};
