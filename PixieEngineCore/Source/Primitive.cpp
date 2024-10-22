#include "pch.h"
#include "Primitive.h"

const Bounds3f Primitive::GetBounds() const {
	return m_bounds;
}

BoundingPrimitive::BoundingPrimitive(const std::vector<Primitive*>& children)
	: m_children(children) {
	for (size_t i = 0; i < children.size(); i++) {
		m_bounds = Union(m_bounds, children[i]->GetBounds());
	}
}

std::optional<ShapeIntersection> BoundingPrimitive::Intersect(const Ray& ray, Float tMax) const {
	if (!m_bounds.IntersectP(ray, tMax)) return {};
	std::optional<ShapeIntersection> intersection;
	for (size_t i = 0; i < m_children.size(); i++) {
		if (auto si = m_children[i]->Intersect(ray, tMax)) {
			intersection = si;
			tMax = si->tHit;
		}
	}
	return intersection;
}

bool BoundingPrimitive::IntersectP(const Ray& ray, Float tMax) const {
	if (!m_bounds.IntersectP(ray, tMax)) return false;
	for (int32_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->IntersectP(ray, tMax)) return true;
	}
	return false;
}

ShapePrimitive::ShapePrimitive(const Shape* shape, const Material* material, const DiffuseAreaLight* areaLight)
	: m_shape(shape), m_material(material), m_areaLight(areaLight) {
	m_bounds = m_shape->Bounds();
}

std::optional<ShapeIntersection> ShapePrimitive::Intersect(const Ray& ray, Float tMax) const {
	RayTracingStatistics::IncrementTriangleTests();
	std::optional<ShapeIntersection> si = m_shape->Intersect(ray, tMax);
	if (si) {
		si->intr.material = m_material;
		si->intr.areaLight = m_areaLight;
	}
	return si;
}

bool ShapePrimitive::IntersectP(const Ray& ray, Float tMax) const {
	RayTracingStatistics::IncrementTriangleTests();
	return m_shape->IntersectP(ray, tMax);
}

BVHPrimitive::BVHPrimitive() {}

BVHPrimitive::BVHPrimitive(size_t primitiveIndex, const Bounds3f& bounds)
	: primitiveIndex(primitiveIndex), bounds(bounds) {}

Vec3 BVHPrimitive::Centroid() const {
	return (Float)0.5f * (bounds.min + bounds.max);
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
		bvhPrimitives[i] = BVHPrimitive(i, primitives[i]->GetBounds());
	}

	std::vector<Primitive*> orderedPrims(primitives.size());
	BVHBuildNode* root;
	std::atomic<int32_t> totalNodes{ 0 };

	std::atomic<int32_t> orderedPrimsOffset{ 0 };
	root = buildRecursive(std::span<BVHPrimitive>(bvhPrimitives), &totalNodes, &orderedPrimsOffset, orderedPrims);

	primitives.swap(orderedPrims);

	bvhPrimitives.resize(0);
	nodes = new LinearBVHNode[totalNodes];
	int32_t offset = 0;
	flattenBVH(root, &offset);
}

Bounds3f BVHAggregate::Bounds() const {
	return nodes[0].bounds;
}

std::optional<ShapeIntersection> BVHAggregate::Intersect(const Ray& ray, Float tMax) const {
	if (!nodes) {
		return {};
	}
	Vec3 invDir(1 / ray.direction.x, 1 / ray.direction.y, 1 / ray.direction.z);
	int32_t dirIsNeg[3] = { int32_t(invDir.x < 0), int32_t(invDir.y < 0), int32_t(invDir.z < 0) };
	int32_t toVisitOffset = 0, currentNodeIndex = 0;
	int32_t nodesToVisit[64];
	std::optional<ShapeIntersection> intersection;
	while (true) {
		const LinearBVHNode* node = &nodes[currentNodeIndex];
		if (node->bounds.IntersectP(ray, tMax)) {
			if (node->nPrimitives > 0) {
				for (int32_t i = 0; i < node->nPrimitives; ++i) {
					std::optional<ShapeIntersection> si = primitives[node->primitivesOffset + i]->Intersect(ray, tMax);
					if (si) {
						intersection = si;
						tMax = si->tHit;
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
	return intersection;
}

bool BVHAggregate::IntersectP(const Ray& ray, Float tMax) const {
	if (!nodes) return false;
	Vec3 invDir(1.f / ray.direction.x, 1.f / ray.direction.y, 1.f / ray.direction.z);
	int32_t dirIsNeg[3] = { int32_t(invDir.x < 0), int32_t(invDir.y < 0), int32_t(invDir.z < 0) };
	int32_t nodesToVisit[64];
	int32_t toVisitOffset = 0, currentNodeIndex = 0;

	while (true) {
		const LinearBVHNode* node = &nodes[currentNodeIndex];
		if (node->bounds.IntersectP(ray, tMax)) {
			if (node->nPrimitives > 0) {
				for (int32_t i = 0; i < node->nPrimitives; ++i) {
					if (primitives[node->primitivesOffset + i]->IntersectP(ray, tMax)) {
						return true;
					}
				}
				if (toVisitOffset == 0) break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else {
				if (dirIsNeg[node->axis] != 0) {
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
	if (bvhPrimitives.size() == 0) return node;
	++*totalNodes;
	Bounds3f bounds;
	for (const auto& prim : bvhPrimitives) {
		bounds = Union(bounds, prim.bounds);
	}

	if (bounds.Area() == 0 || bvhPrimitives.size() == 1) {
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

		if (centroidBounds.max[dim] == centroidBounds.min[dim]) {
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
					costs[i] += countBelow * boundBelow.Area();
				}

				int32_t countAbove = 0;
				Bounds3f boundAbove;
				for (int32_t i = nSplits; i >= 1; --i) {
					boundAbove = Union(boundAbove, buckets[i].bounds);
					countAbove += buckets[i].count;
					costs[i - 1] += countAbove * boundAbove.Area();
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
				minCost = 1.f / 2.f + minCost / bounds.Area();

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