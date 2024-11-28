#pragma once
#include "pch.h"
#include "MathAndPhysics.h"
#include "Ray.h"

inline bool IsAABBIntersected(const Ray& ray, Vec3 pMin, Vec3 pMax, Float maxDistance, Float* tHit0, Float* tHit1) {
    Float t0 = 0, t1 = maxDistance;
    for (int32_t i = 0; i < 3; ++i) {
        Float tNear = (pMin[i] - ray.origin[i]) * ray.inverseDirection[i];
        Float tFar = (pMax[i] - ray.origin[i]) * ray.inverseDirection[i];

        if (tNear > tFar) {
            std::swap(tNear, tFar);
        }

        tFar *= 1 + 2 * gamma(3);

        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1)
            return false;
    }
    if (tHit0) {
        *tHit0 = t0;
    }
    if (tHit1) {
        *tHit1 = t1;
    }
    return true;
}

struct BVHNode {
	Vec3 pMin = Vec3(Infinity);
	int32_t childOffset = -1;
	Vec3 pMax = Vec3(-Infinity);
	int16_t nTriangles = 0;
	int16_t axis = 0;

	BVHNode() = default;
	BVHNode(int32_t childOffset, int16_t nTriangles, int8_t axis) :
		childOffset(childOffset), nTriangles(nTriangles), axis(axis) {}
	BVHNode(Vec3 pMin, Vec3 pMax, int8_t axis) :
		pMin(pMin), pMax(pMax), axis(axis) {}

    bool IsIntersected(const Ray& ray, Float maxDistance, Float* tHit0 = nullptr, Float* tHit1 = nullptr) {
        return IsAABBIntersected(ray, pMin, pMax, maxDistance, tHit0, tHit1);
    }
};

enum class ObjectType : int32_t {
    Node = 0,
    Sphere,
    Mesh,
};

constexpr int32_t ObjectBVHNodeChildrenCount = 2;

struct ObjectBVHNode {
    Transform transform;
    Vec3 pMin;
    Vec3 pMax;
    ObjectType childrenTypes[ObjectBVHNodeChildrenCount] = { ObjectType::Node , ObjectType::Node };
    int32_t childrenIndexes[ObjectBVHNodeChildrenCount] = {-1, -1};

    ObjectBVHNode(Transform transform, Vec3 pMin, Vec3 pMax) :
        transform(transform), pMin(pMin), pMax(pMax) {}

    bool IsIntersected(const Ray& ray, Float maxDistance, Float* tHit0 = nullptr, Float* tHit1 = nullptr) {
        return IsAABBIntersected(ray, pMin, pMax, maxDistance, tHit0, tHit1);
    }
};

