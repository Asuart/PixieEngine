#pragma once
#include "Ray.h"
#include "RTInteraction.h"
#include "Bounds.h"
#include "Shape.h"

class Primitive {
public:
	Bounds3f bounds;

	virtual bool Intersect(const Ray& ray, RTInteraction& outCollision, Float tMax = Infinity) const = 0;
	virtual bool IntersectP(const Ray& ray, Float tMax = Infinity) const = 0;
};

class BoundingPrimitive : public Primitive {
public:
	std::vector<Primitive*> children = std::vector<Primitive*>();

	BoundingPrimitive(const std::vector<Primitive*>& _children);

	bool Intersect(const Ray& ray, RTInteraction& outCollision, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, Float tMax = Infinity) const override;
};

class ShapePrimitive : public Primitive {
public:
	RTMaterial* material;
	Shape* shape;

	ShapePrimitive(Shape* _shape, RTMaterial* _material);

	bool Intersect(const Ray& ray, RTInteraction& outCollision, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, Float tMax = Infinity) const override;
};

struct BVHSplitBucket {
	int count = 0;
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

	void InitLeaf(int first, int n, const Bounds3f& b);
	void InitInterior(int axis, BVHBuildNode* c0, BVHBuildNode* c1);
};

struct alignas(32) LinearBVHNode {
	Bounds3f bounds;
	union {
		int primitivesOffset;
		int secondChildOffset;
	};
	uint16_t nPrimitives;
	uint8_t axis;
};

class BVHAggregate : public Primitive {
public:
	BVHAggregate(std::vector<Primitive*> p, int maxPrimsInNode = 1);

	Bounds3f Bounds() const;
	bool Intersect(const Ray& ray, RTInteraction& outCollision, Float tMax = Infinity) const override;
	bool IntersectP(const Ray& ray, Float tMax = Infinity) const override;

	int maxPrimsInNode;
	std::vector<Primitive*> primitives;
	LinearBVHNode* nodes = nullptr;
private:
	BVHBuildNode* buildRecursive(std::span<BVHPrimitive> bvhPrimitives, std::atomic<int>* totalNodes, std::atomic<int>* orderedPrimsOffset, std::vector<Primitive*>& orderedPrims);
	int flattenBVH(BVHBuildNode* node, int* offset);
};
