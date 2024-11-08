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
	if (!RayTracing::IsIntersected(ray, m_bounds, tMax)) return {};
	std::optional<ShapeIntersection> intersection;
	for (size_t i = 0; i < m_children.size(); i++) {
		if (auto si = m_children[i]->Intersect(ray, tMax)) {
			intersection = si;
			tMax = si->tHit;
		}
	}
	return intersection;
}

bool BoundingPrimitive::IsIntersected(const Ray& ray, Float tMax) const {
	if (!RayTracing::IsIntersected(ray, m_bounds, tMax)) return false;
	for (int32_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->IsIntersected(ray, tMax)) return true;
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

bool ShapePrimitive::IsIntersected(const Ray& ray, Float tMax) const {
	RayTracingStatistics::IncrementTriangleTests();
	return m_shape->IsIntersected(ray, tMax);
}

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

BVHAggregate::BVHAggregate(std::vector<Primitive*> prims, int32_t maxPrimsInNode, SplitMethod splitMethod)
	: maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(std::move(prims)), splitMethod(splitMethod) {
	std::vector<BVHPrimitive> bvhPrimitives(primitives.size());
	for (size_t i = 0; i < primitives.size(); i++) {
		bvhPrimitives[i] = BVHPrimitive(i, primitives[i]->GetBounds());
	}

	using Resource = std::pmr::monotonic_buffer_resource;
	Resource resource;
	std::vector<std::unique_ptr<Resource>> threadBufferResources;

	std::vector<Primitive*> orderedPrims(primitives.size());
	BVHBuildNode* root;
	std::atomic<int32_t> totalNodes{ 0 };
	std::atomic<int32_t> orderedPrimsOffset{ 0 };
	root = BuildRecursive(std::span<BVHPrimitive>(bvhPrimitives), &totalNodes, &orderedPrimsOffset, orderedPrims);
	primitives.swap(orderedPrims);

	bvhPrimitives.resize(0);
	nodes = new LinearBVHNode[totalNodes];
	int32_t offset = 0;
	flattenBVH(root, &offset);
	m_bounds = nodes[0].bounds;
}

Bounds3f BVHAggregate::Bounds() const {
	return nodes[0].bounds;
}

std::optional<ShapeIntersection> BVHAggregate::Intersect(const Ray& ray, Float tMax) const {
	if (!nodes) {
		return {};
	}
	std::optional<ShapeIntersection> si;
	int32_t dirIsNeg[3] = { int32_t(ray.inverseDirection.x < 0), int32_t(ray.inverseDirection.y < 0), int32_t(ray.inverseDirection.z < 0) };
	int32_t toVisitOffset = 0, currentNodeIndex = 0;
	int32_t nodesToVisit[64];
	int32_t nodesVisited = 0;
	while (true) {
		++nodesVisited;
		const LinearBVHNode* node = &nodes[currentNodeIndex];
		if (RayTracing::IsIntersected(ray, node->bounds, tMax, dirIsNeg)) {
			if (node->nPrimitives > 0) {
				for (int32_t i = 0; i < node->nPrimitives; i++) {
					std::optional<ShapeIntersection> primSi = primitives[node->primitivesOffset + i]->Intersect(ray, tMax);
					if (primSi) {
						si = primSi;
						tMax = si->tHit;
					}
				}
				if (toVisitOffset == 0) {
					break;
				}
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
			if (toVisitOffset == 0) {
				break;
			}
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	return si;
}

bool BVHAggregate::IsIntersected(const Ray& ray, Float tMax) const {
	if (!nodes) {
		return false;
	}
	int32_t dirIsNeg[3] = { static_cast<int32_t>(ray.inverseDirection.x < 0), static_cast<int32_t>(ray.inverseDirection.y < 0), static_cast<int32_t>(ray.inverseDirection.z < 0) };
	int32_t nodesToVisit[64];
	int32_t toVisitOffset = 0, currentNodeIndex = 0;
	int32_t nodesVisited = 0;
	while (true) {
		++nodesVisited;
		const LinearBVHNode* node = &nodes[currentNodeIndex];
		if (RayTracing::IsIntersected(ray, node->bounds, tMax, dirIsNeg)) {
			if (node->nPrimitives > 0) {
				for (int32_t i = 0; i < node->nPrimitives; i++) {
					if (primitives[node->primitivesOffset + i]->IsIntersected(ray, tMax)) {
						return true;
					}
				}
				if (toVisitOffset == 0)
					break;
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
			if (toVisitOffset == 0) {
				break;
			}
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	return false;
}

BVHBuildNode* BVHAggregate::BuildRecursive(std::span<BVHPrimitive> bvhPrimitives, std::atomic<int32_t>* totalNodes, std::atomic<int32_t>* orderedPrimsOffset, std::vector<Primitive*>& orderedPrims) {
    BVHBuildNode* node = new BVHBuildNode();
    ++*totalNodes;
    Bounds3f bounds;
    for (const auto& prim : bvhPrimitives) {
        bounds = Union(bounds, prim.bounds);
    }

    if (bounds.Area() == 0 || bvhPrimitives.size() == 1) {
        int32_t firstPrimOffset = orderedPrimsOffset->fetch_add(bvhPrimitives.size());
        for (size_t i = 0; i < bvhPrimitives.size(); i++) {
            int32_t index = bvhPrimitives[i].primitiveIndex;
            orderedPrims[firstPrimOffset + i] = primitives[index];
        }
        node->InitLeaf(firstPrimOffset, bvhPrimitives.size(), bounds);
        return node;
    }
    else {
        Bounds3f centroidBounds;
        for (const auto& prim : bvhPrimitives) {
            centroidBounds = Union(centroidBounds, prim.Centroid());
        }
        int32_t dim = centroidBounds.MaxDimension();

        if (centroidBounds.max[dim] == centroidBounds.min[dim]) {
            int32_t firstPrimOffset = orderedPrimsOffset->fetch_add(bvhPrimitives.size());
            for (size_t i = 0; i < bvhPrimitives.size(); i++) {
                int32_t index = bvhPrimitives[i].primitiveIndex;
                orderedPrims[firstPrimOffset + i] = primitives[index];
            }
            node->InitLeaf(firstPrimOffset, bvhPrimitives.size(), bounds);
            return node;
        }
        else {
            int32_t mid = bvhPrimitives.size() / 2;
			switch (splitMethod) {
			case SplitMethod::Middle: {
				Float pmid = (centroidBounds.min[dim] + centroidBounds.max[dim]) / 2;
				auto midIter = std::partition(bvhPrimitives.begin(), bvhPrimitives.end(),
					[dim, pmid](const BVHPrimitive& pi) {
						return pi.Centroid()[dim] < pmid;
					});
				mid = midIter - bvhPrimitives.begin();
				if (midIter != bvhPrimitives.begin() && midIter != bvhPrimitives.end()) {
					break;
				}
			}
			case SplitMethod::EqualCounts: {
				mid = bvhPrimitives.size() / 2;
				std::nth_element(bvhPrimitives.begin(), bvhPrimitives.begin() + mid, bvhPrimitives.end(),
					[dim](const BVHPrimitive& a, const BVHPrimitive& b) {
						return a.Centroid()[dim] < b.Centroid()[dim];
					});
				break;
			}
			case SplitMethod::SAH:
			default: {
				if (bvhPrimitives.size() <= 2) {
					mid = bvhPrimitives.size() / 2;
					std::nth_element(bvhPrimitives.begin(), bvhPrimitives.begin() + mid, bvhPrimitives.end(),
						[dim](const BVHPrimitive& a, const BVHPrimitive& b) {
							return a.Centroid()[dim] < b.Centroid()[dim];
						});
				}
				else {
					constexpr int32_t nBuckets = 12;
					BVHSplitBucket buckets[nBuckets];

					for (const auto& prim : bvhPrimitives) {
						int32_t b = nBuckets * centroidBounds.Offset(prim.Centroid())[dim];
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
					for (int32_t i = 0; i < nSplits; i++) {
						boundBelow = Union(boundBelow, buckets[i].bounds);
						countBelow += buckets[i].count;
						costs[i] += countBelow * boundBelow.Area();
					}

					int32_t countAbove = 0;
					Bounds3f boundAbove;
					for (int32_t i = nSplits; i >= 1; i--) {
						boundAbove = Union(boundAbove, buckets[i].bounds);
						countAbove += buckets[i].count;
						costs[i - 1] += countAbove * boundAbove.Area();
					}

					int32_t minCostSplitBucket = -1;
					Float minCost = Infinity;
					for (int32_t i = 0; i < nSplits; i++) {
						if (costs[i] < minCost) {
							minCost = costs[i];
							minCostSplitBucket = i;
						}
					}

					Float leafCost = bvhPrimitives.size();
					minCost = 1.f / 2.f + minCost / bounds.Area();

					if (bvhPrimitives.size() > maxPrimsInNode || minCost < leafCost) {
						auto midIter = std::partition(bvhPrimitives.begin(), bvhPrimitives.end(),
							[=](const BVHPrimitive& bp) {
								int32_t b = nBuckets * centroidBounds.Offset(bp.Centroid())[dim];
								if (b == nBuckets) {
									b = nBuckets - 1;
								}
								return b <= minCostSplitBucket;
							});
						mid = midIter - bvhPrimitives.begin();
					}
					else {
						int32_t firstPrimOffset = orderedPrimsOffset->fetch_add(bvhPrimitives.size());
						for (size_t i = 0; i < bvhPrimitives.size(); i++) {
							int32_t index = bvhPrimitives[i].primitiveIndex;
							orderedPrims[firstPrimOffset + i] = primitives[index];
						}
						node->InitLeaf(firstPrimOffset, bvhPrimitives.size(), bounds);
						return node;
					}
				}
				break;
			}
			}

			if (bvhPrimitives.size() == 0) {
				return node;
			}

            BVHBuildNode* children[2];
			if (false /*bvhPrimitives.size() > 128 * 1024*/) {
				//ParallelFor(0, 2, [&](int32_t i) {
				//	if (i == 0) {
				//		children[0] = BuildRecursive(bvhPrimitives.subspan(0, mid), totalNodes, orderedPrimsOffset, orderedPrims);
				//	}
				//	else {
				//		children[1] = BuildRecursive(bvhPrimitives.subspan(mid), totalNodes, orderedPrimsOffset, orderedPrims);
				//	}
				//	});
			}
            else {
                children[0] = BuildRecursive(bvhPrimitives.subspan(0, mid), totalNodes, orderedPrimsOffset, orderedPrims);
                children[1] = BuildRecursive(bvhPrimitives.subspan(mid), totalNodes, orderedPrimsOffset, orderedPrims);
            }

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
		if (node->children[0]) {
			flattenBVH(node->children[0], offset);
		}
		if (node->children[1]) {
			linearNode->secondChildOffset = flattenBVH(node->children[1], offset);
		}
	}
	return nodeOffset;
}
