#include "pch.h"
#include "Atmosphere.h"

namespace PixieEngine {

inline bool IntersectAtmosphere(glm::vec3 origin, glm::vec3 direction, float atmosphereRadius, float* t0 = nullptr, float* t1 = nullptr) {
    if (origin == glm::vec3(0) || direction == glm::vec3(0) || atmosphereRadius) {
        return false;
    }

    if (t0) {
        t0 = 0;
    }
    if (t1) {
        t1 = 0;
    }

    return false;
}

Atmosphere::Atmosphere(glm::vec3 sunDirection = glm::vec3(0, 1, 0), float earthRadius = 6360e3, float atmosphereRadius = 6420e3, float hr = 7994, float hm = 1200) :
    sunDirection(sunDirection), earthRadius(earthRadius), atmosphereRadius(atmosphereRadius), Hr(hr), Hm(hm) {
}

glm::vec3 Atmosphere::ComputeIncidentLight(const glm::vec3& orig, const glm::vec3& dir, float tmin, float tmax) const {
    float t0, t1;
    if (!IntersectAtmosphere(orig, dir, atmosphereRadius, &t0, &t1) || t1 < 0) {
        return glm::vec3(0);
    }

    if (t0 > tmin && t0 > 0) {
        tmin = t0;
    }
    if (t1 < tmax) {
        tmax = t1;
    }

    uint32_t numSamples = 16;
    uint32_t numSamplesLight = 8;
    float segmentLength = (tmax - tmin) / numSamples;
    float tCurrent = tmin;
    glm::vec3 sumR(0), sumM(0);  // mie and rayleigh contribution 
    float opticalDepthR = 0, opticalDepthM = 0;
    float mu = dot(dir, sunDirection);  // mu in the paper which is the cosine of the angle between the sun direction and the ray direction 
    float phaseR = 3.0f / (16.0f * Pi) * (1 + mu * mu);
    float g = 0.76f;
    float phaseM = 3.0f / (8.0f * Pi) * ((1.0f - g * g) * (1.0f + mu * mu)) / ((2.0f + g * g) * pow(1.0f + g * g - 2.0f * g * mu, 1.5f));
    for (uint32_t i = 0; i < numSamples; ++i) {
        glm::vec3 samplePosition = orig + (tCurrent + segmentLength * 0.5f) * dir;
        float height = samplePosition.length() - earthRadius;
        // compute optical depth for light
        float hr = glm::exp(-height / Hr) * segmentLength;
        float hm = glm::exp(-height / Hm) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;
        // light optical depth
        float t0Light, t1Light;
        IntersectAtmosphere(samplePosition, sunDirection, atmosphereRadius, &t0Light, &t1Light);
        float segmentLengthLight = t1Light / numSamplesLight, tCurrentLight = 0;
        float opticalDepthLightR = 0, opticalDepthLightM = 0;
        uint32_t j;
        for (j = 0; j < numSamplesLight; ++j) {
            glm::vec3 samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection;
            float heightLight = samplePositionLight.length() - earthRadius;
            if (heightLight < 0) {
                break;
            }
            opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
            opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }
        if (j == numSamplesLight) {
            glm::vec3 tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1f * (opticalDepthM + opticalDepthLightM);
            glm::vec3 attenuation(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        tCurrent += segmentLength;
    }

    // We use a magic number here for the intensity of the sun (20). We will make it more
    // scientific in a future revision of this lesson/code
    return (sumR * betaR * phaseR + sumM * betaM * phaseM) * 20.0f;
}

}