#pragma once
#include "Ray.h"
#include "Interaction.h"
#include "Bounds.h"
#include "Triangle.h"
#include "RayTracingStatistics.h"
#include "DiffuseAreaLight.h"
#include "RayTracing.h"

class Primitive {
public:
	virtual std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const = 0;
	virtual bool IsIntersected(const Ray& ray, Float tMax = Infinity) const = 0;
	
	const Bounds3f GetBounds() const;

protected:
	Bounds3f m_bounds;
};

class BoundingPrimitive : public Primitive {
public:
	BoundingPrimitive(const std::vector<Primitive*>& children);

	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IsIntersected(const Ray& ray, Float tMax = Infinity) const override;

protected:
	std::vector<Primitive*> m_children = std::vector<Primitive*>();
};

class ShapePrimitive : public Primitive {
public:
	ShapePrimitive(const Shape* shape, const Material* material, const DiffuseAreaLight* areaLight = nullptr);

	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax = Infinity) const override;
	bool IsIntersected(const Ray& ray, Float tMax = Infinity) const override;

protected:
	const Shape* m_shape = nullptr;
	const Material* m_material = nullptr;
	const DiffuseAreaLight* m_areaLight = nullptr;
};

struct BVHSplitBucket {
	int32_t count = 0;
	Bounds3f bounds;
};

struct BVHPrimitive {
	size_t primitiveIndex = 0;
	Bounds3f bounds = Bounds3f();

	BVHPrimitive() = default;
	BVHPrimitive(size_t primitiveIndex, const Bounds3f& bounds);

	Vec3 Centroid() const;
};

struct BVHBuildNode {
	Bounds3f bounds;
	BVHBuildNode* children[2];
	int32_t splitAxis, firstPrimOffset, nPrimitives;

	void InitLeaf(int32_t first, int32_t n, const Bounds3f& b);
	void InitInterior(int32_t axis, BVHBuildNode* c0, BVHBuildNode* c1);
};

struct alignas(32) LinearBVHNode {
	Bounds3f bounds;
	union {
		int32_t primitivesOffset;
		int32_t secondChildOffset;
	};
	uint16_t nPrimitives = 0;
	uint8_t axis = 0;
};

struct MortonPrimitive {
	int32_t primitiveIndex;
	uint32_t mortonCode;
};

class BVHAggregate : public Primitive {
public:
	enum class SplitMethod { SAH, Middle, EqualCounts };

	BVHAggregate(std::vector<Primitive*> p, int32_t maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::SAH);

	Bounds3f Bounds() const;
	std::optional<ShapeIntersection> Intersect(const Ray& ray, Float tMax) const;
	bool IsIntersected(const Ray& ray, Float tMax) const;

private:
	BVHBuildNode* BuildRecursive(std::span<BVHPrimitive> bvhPrimitives, std::atomic<int32_t>* totalNodes, std::atomic<int32_t>* orderedPrimsOffset, std::vector<Primitive*>& orderedPrims);
	int32_t flattenBVH(BVHBuildNode* node, int32_t* offset);

	int32_t maxPrimsInNode;
	std::vector<Primitive*> primitives;
	SplitMethod splitMethod;
	LinearBVHNode* nodes = nullptr;
};
