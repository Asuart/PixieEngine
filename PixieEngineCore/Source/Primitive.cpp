
#include "Primitive.h"

BoundingPrimitive::BoundingPrimitive(const std::vector<Primitive*>& _children)
	: children(_children) {
	for (int32_t i = 0; i < children.size(); i++) {
		bounds = Union(bounds, children[i]->bounds);
	}
}

bool BoundingPrimitive::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	if (!bounds.IntersectP(ray, stats, tMax)) return false;
	bool intersected = false;
	for (int32_t i = 0; i < children.size(); i++) {
		bool intr = children[i]->Intersect(ray, outCollision, stats, tMax);
		if (intr) {
			intersected = true;
			tMax = outCollision.distance;
		}
	}
	return intersected;
}

bool BoundingPrimitive::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	if (!bounds.IntersectP(ray, stats, tMax)) return false;
	for (int32_t i = 0; i < children.size(); i++) {
		if (children[i]->IntersectP(ray, stats, tMax)) return true;
	}
	return false;
}

ShapePrimitive::ShapePrimitive(Shape* _shape, const Material* _material, const AreaLight* _areaLight)
	: shape(_shape), material(_material), areaLight(_areaLight) {
	bounds = shape->Bounds();
}

bool ShapePrimitive::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	if (!bounds.IntersectP(ray, stats, tMax)) return false;
	bool col = shape->Intersect(ray, outCollision, stats, tMax);
	if (col) {
		outCollision.material = material;
		outCollision.areaLight = areaLight;
	}
	return col;
}

bool ShapePrimitive::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	if (!bounds.IntersectP(ray, stats, tMax)) return false;
	return shape->IntersectP(ray, stats, tMax);
}

BVHPrimitive::BVHPrimitive() {}

BVHPrimitive::BVHPrimitive(size_t primitiveIndex, const Bounds3f& bounds)
	: primitiveIndex(primitiveIndex), bounds(bounds) {}

Vec3 BVHPrimitive::Centroid() const {
	return (Float)0.5f * (bounds.pMin + bounds.pMax);
}

void BVHBuildNode::InitLeaf(int32_t first, int32_t n, const Bounds3f& b) {
	firstPrimOffset = first;
	nPrimitives = n;
	bounds = b;
	children[0] = children[1] = nullptr;
}

void BVHBuildNode::InitInterior(int32_t axis, BVHBuildNode* c0, BVHBuildNode* c1) {
	children[0] = c0;
	children[1] = c1;
	bounds = Union(c0->bounds, c1->bounds);
	splitAxis = axis;
	nPrimitives = 0;
}

BVHAggregate::BVHAggregate(std::vector<Primitive*> prims, int32_t maxPrimsInNode)
	: maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(std::move(prims)) {
	std::vector<BVHPrimitive> bvhPrimitives(primitives.size());
	for (size_t i = 0; i < primitives.size(); ++i) {
		bvhPrimitives[i] = BVHPrimitive(i, primitives[i]->bounds);
	}

	std::vector<Primitive*> orderedPrims(primitives.size());
	BVHBuildNode* root;
	// Build BVH according to selected _splitMethod_
	std::atomic<int32_t> totalNodes{ 0 };

	std::atomic<int32_t> orderedPrimsOffset{ 0 };
	root = buildRecursive(std::span<BVHPrimitive>(bvhPrimitives), &totalNodes, &orderedPrimsOffset, orderedPrims);

	primitives.swap(orderedPrims);

	// Convert BVH into compact representation in _nodes_ array
	bvhPrimitives.resize(0);
	nodes = new LinearBVHNode[totalNodes];
	int32_t offset = 0;
	flattenBVH(root, &offset);
}

Bounds3f BVHAggregate::Bounds() const {
	return nodes[0].bounds;
}

bool BVHAggregate::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	if (!nodes) return false;
	Vec3 invDir(1 / ray.direction.x, 1 / ray.direction.y, 1 / ray.direction.z);
	int32_t dirIsNeg[3] = { int32_t(invDir.x < 0), int32_t(invDir.y < 0), int32_t(invDir.z < 0) };
	int32_t toVisitOffset = 0, currentNodeIndex = 0;
	int32_t nodesToVisit[64];
	bool collided = false;
	while (true) {
		const LinearBVHNode* node = &nodes[currentNodeIndex];
		if (node->bounds.IntersectP(ray, stats, tMax)) {
			if (node->nPrimitives > 0) {
				for (int32_t i = 0; i < node->nPrimitives; ++i) {
					bool col = primitives[node->primitivesOffset + i]->Intersect(ray, outCollision, stats, tMax);
					if (col) {
						collided = true;
						tMax = outCollision.distance;
					}
				}
				if (toVisitOffset == 0) break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else {
				if (dirIsNeg[node->axis]) {
					nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
					currentNodeIndex = node->secondChildOffset;
				}
				else {
					nodesToVisit[toVisitOffset++] = node->secondChildOffset;
					currentNodeIndex = currentNodeIndex + 1;
				}
			}
		}
		else {
			if (toVisitOffset == 0) break;
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	return collided;
}

bool BVHAggregate::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	if (!nodes) return false;
	Vec3 invDir(1.f / ray.direction.x, 1.f / ray.direction.y, 1.f / ray.direction.z);
	int32_t dirIsNeg[3] = { int32_t(invDir.x < 0), int32_t(invDir.y < 0), int32_t(invDir.z < 0) };
	int32_t nodesToVisit[64];
	int32_t toVisitOffset = 0, currentNodeIndex = 0;

	while (true) {
		const LinearBVHNode* node = &nodes[currentNodeIndex];
		if (node->bounds.IntersectP(ray, stats, tMax)) {
			// Process BVH node _node_ for traversal
			if (node->nPrimitives > 0) {
				for (int32_t i = 0; i < node->nPrimitives; ++i) {
					if (primitives[node->primitivesOffset + i]->IntersectP(ray, stats, tMax)) {
						return true;
					}
				}
				if (toVisitOffset == 0) break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else {
				if (dirIsNeg[node->axis] != 0) {
					/// second child first
					nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
					currentNodeIndex = node->secondChildOffset;
				}
				else {
					nodesToVisit[toVisitOffset++] = node->secondChildOffset;
					currentNodeIndex = currentNodeIndex + 1;
				}
			}
		}
		else {
			if (toVisitOffset == 0) break;
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	return false;
}

BVHBuildNode* BVHAggregate::buildRecursive(std::span<BVHPrimitive> bvhPrimitives, std::atomic<int32_t>* totalNodes, std::atomic<int32_t>* orderedPrimsOffset, std::vector<Primitive*>& orderedPrims) {
	BVHBuildNode* node = new BVHBuildNode();
	++*totalNodes;
	Bounds3f bounds;
	for (const auto& prim : bvhPrimitives) {
		bounds = Union(bounds, prim.bounds);
	}

	if (bounds.SurfaceArea() == 0 || bvhPrimitives.size() == 1) {
		size_t firstPrimOffset = orderedPrimsOffset->fetch_add((int32_t)bvhPrimitives.size());
		for (size_t i = 0; i < bvhPrimitives.size(); ++i) {
			size_t index = bvhPrimitives[i].primitiveIndex;
			orderedPrims[firstPrimOffset + i] = primitives[index];
		}
		node->InitLeaf((int32_t)firstPrimOffset, (int32_t)bvhPrimitives.size(), bounds);
		return node;
	}
	else {
		Bounds3f centroidBounds;
		for (const auto& prim : bvhPrimitives) {
			centroidBounds = Union(centroidBounds, Bounds3f(prim.Centroid()));
		}
		int32_t dim = centroidBounds.MaxDimension();

		if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim]) {
			size_t firstPrimOffset = orderedPrimsOffset->fetch_add((int32_t)bvhPrimitives.size());
			for (size_t i = 0; i < bvhPrimitives.size(); ++i) {
				size_t index = bvhPrimitives[i].primitiveIndex;
				orderedPrims[firstPrimOffset + i] = primitives[index];
			}
			node->InitLeaf((int32_t)firstPrimOffset, (int32_t)bvhPrimitives.size(), bounds);
			return node;
		}
		else {
			size_t mid = bvhPrimitives.size() / 2;

			if (bvhPrimitives.size() <= 2) {
				mid = bvhPrimitives.size() / 2;
				std::nth_element(bvhPrimitives.begin(), bvhPrimitives.begin() + mid, bvhPrimitives.end(),
					[dim](const BVHPrimitive& a, const BVHPrimitive& b) {
						return a.Centroid()[dim] < b.Centroid()[dim];
					});
			}
			else {
				constexpr uint32_t nBuckets = 12;
				BVHSplitBucket buckets[nBuckets];

				for (const auto& prim : bvhPrimitives) {
					int32_t b = (int32_t)(nBuckets * centroidBounds.Offset(prim.Centroid())[dim]);
					if (b == nBuckets) {
						b = nBuckets - 1;
					}
					buckets[b].count++;
					buckets[b].bounds = Union(buckets[b].bounds, prim.bounds);
				}

				constexpr int32_t nSplits = nBuckets - 1;
				Float costs[nSplits] = {};

				int32_t countBelow = 0;
				Bounds3f boundBelow;
				for (int32_t i = 0; i < nSplits; ++i) {
					boundBelow = Union(boundBelow, buckets[i].bounds);
					countBelow += buckets[i].count;
					costs[i] += countBelow * boundBelow.SurfaceArea();
				}

				int32_t countAbove = 0;
				Bounds3f boundAbove;
				for (int32_t i = nSplits; i >= 1; --i) {
					boundAbove = Union(boundAbove, buckets[i].bounds);
					countAbove += buckets[i].count;
					costs[i - 1] += countAbove * boundAbove.SurfaceArea();
				}

				int32_t minCostSplitBucket = -1;
				Float minCost = Infinity;
				for (int32_t i = 0; i < nSplits; ++i) {
					if (costs[i] < minCost) {
						minCost = costs[i];
						minCostSplitBucket = i;
					}
				}

				Float leafCost = (Float)bvhPrimitives.size();
				minCost = 1.f / 2.f + minCost / bounds.SurfaceArea();

				if (bvhPrimitives.size() > maxPrimsInNode || minCost < leafCost) {
					auto midIter = std::partition(
						bvhPrimitives.begin(), bvhPrimitives.end(),
						[=](const BVHPrimitive& bp) {
							int32_t b = (int32_t)(nBuckets * centroidBounds.Offset(bp.Centroid())[dim]);
							if (b == nBuckets) b = nBuckets - 1;
							return b <= minCostSplitBucket;
						});
					mid = midIter - bvhPrimitives.begin();
				}
				else {
					int32_t firstPrimOffset = orderedPrimsOffset->fetch_add((int32_t)bvhPrimitives.size());
					for (size_t i = 0; i < bvhPrimitives.size(); ++i) {
						size_t index = bvhPrimitives[i].primitiveIndex;
						orderedPrims[firstPrimOffset + i] = primitives[index];
					}
					node->InitLeaf(firstPrimOffset, (int32_t)bvhPrimitives.size(), bounds);
					return node;
				}
			}

			BVHBuildNode* children[2];

			children[0] = buildRecursive(bvhPrimitives.subspan(0, mid), totalNodes, orderedPrimsOffset, orderedPrims);
			children[1] = buildRecursive(bvhPrimitives.subspan(mid), totalNodes, orderedPrimsOffset, orderedPrims);
			node->InitInterior(dim, children[0], children[1]);
		}
	}
	return node;
}

int32_t BVHAggregate::flattenBVH(BVHBuildNode* node, int32_t* offset) {
	LinearBVHNode* linearNode = &nodes[*offset];
	linearNode->bounds = node->bounds;
	int32_t nodeOffset = (*offset)++;
	if (node->nPrimitives > 0) {
		linearNode->primitivesOffset = node->firstPrimOffset;
		linearNode->nPrimitives = node->nPrimitives;
	}
	else {
		linearNode->axis = node->splitAxis;
		linearNode->nPrimitives = 0;
		flattenBVH(node->children[0], offset);
		linearNode->secondChildOffset = flattenBVH(node->children[1], offset);
	}
	return nodeOffset;
}