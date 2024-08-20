#pragma once
#include "Ray.h"
#include "Interaction.h"
#include "Bounds.h"
#include "TriangleCache.h"
#include "RayTracingStatistics.h"

class Primitive {
public:
	virtual bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax = Infinity) const = 0;
	virtual bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity) const = 0;
	
	const Bounds3f GetBounds() const;

protected:
	Bounds3f m_bounds;
};

class BoundingPrimitive : public Primitive {
public:
	BoundingPrimitive(const std::vector<Primitive*>& children);

	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity) const override;

protected:
	std::vector<Primitive*> m_children = std::vector<Primitive*>();
};

class TrianglePrimitive : public Primitive {
public:
	TrianglePrimitive(const TriangleCache& triangle, const Material* material, const AreaLight* areaLight = nullptr);

	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity) const override;

protected:
	const TriangleCache m_triangle;
	const Material* m_material = nullptr;
	const AreaLight* m_areaLight = nullptr;
};

struct BVHSplitBucket {
	int32_t count = 0;
	Bounds3f bounds;
};

struct BVHPrimitive {
	size_t primitiveIndex;
	Bounds3f bounds;

	BVHPrimitive();
	BVHPrimitive(size_t primitiveIndex, const Bounds3f& bounds);

	Vec3 Centroid() const;
};

struct BVHBuildNode {
	Bounds3f bounds;
	BVHBuildNode* children[2];
	int splitAxis, firstPrimOffset, nPrimitives;

	void InitLeaf(int32_t first, int32_t n, const Bounds3f& b);
	void InitInterior(int32_t axis, BVHBuildNode* c0, BVHBuildNode* c1);
};

struct alignas(32) LinearBVHNode {
	Bounds3f bounds;
	union {
		int32_t primitivesOffset;
		int32_t secondChildOffset;
	};
	uint16_t nPrimitives;
	uint8_t axis;
};

class BVHAggregate : public Primitive {
public:
	BVHAggregate(std::vector<Primitive*> p, int32_t maxPrimsInNode = 1);

	Bounds3f Bounds() const;
	bool Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax = Infinity) const override;

	int32_t maxPrimsInNode;
	std::vector<Primitive*> primitives;
	LinearBVHNode* nodes = nullptr;
private:
	BVHBuildNode* buildRecursive(std::span<BVHPrimitive> bvhPrimitives, std::atomic<int32_t>* totalNodes, std::atomic<int32_t>* orderedPrimsOffset, std::vector<Primitive*>& orderedPrims);
	int32_t flattenBVH(BVHBuildNode* node, int32_t* offset);
};
