#pragma once
#include "pch.h"
#include "Math/MathBase.h"

namespace PixieEngine {

class Atmosphere {
public:
    Atmosphere(glm::vec3 sunDirection = glm::vec3(0, 1, 0), float earthRadius = 6360e3, float atmosphereRadius = 6420e3, float hr = 7994, float hm = 1200);

    glm::vec3 ComputeIncidentLight(const glm::vec3& orig, const glm::vec3& dir, float tmin, float tmax) const;

private:
    glm::vec3 sunDirection;
    float earthRadius;
    float atmosphereRadius;
    float Hr;
    float Hm;

public:
    inline static const glm::vec3 betaR = glm::vec3(3.8e-6f, 13.5e-6f, 33.1e-6f);
    inline static const glm::vec3 betaM = glm::vec3(21e-6f);
};

}