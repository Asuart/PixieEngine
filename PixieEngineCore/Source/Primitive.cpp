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

bool BoundingPrimitive::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	if (!m_bounds.IntersectP(ray, stats, tMax)) return false;
	bool intersected = false;
	for (size_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->Intersect(ray, outCollision, stats, tMax)) {
			intersected = true;
			tMax = outCollision.distance;
		}
	}
	return intersected;
}

bool BoundingPrimitive::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	if (!m_bounds.IntersectP(ray, stats, tMax)) return false;
	for (int32_t i = 0; i < m_children.size(); i++) {
		if (m_children[i]->IntersectP(ray, stats, tMax)) return true;
	}
	return false;
}

TrianglePrimitive::TrianglePrimitive(const TriangleCache& triangle, const Material* material, const AreaLight* areaLight)
	: m_triangle(triangle), m_material(material), m_areaLight(areaLight) {
	m_bounds = Bounds3f(glm::min(triangle.p0, triangle.p1, triangle.p2), glm::max(triangle.p0, triangle.p1, triangle.p2));
}

bool TrianglePrimitive::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	stats.m_triangleCheckStatBuffer.Increment(ray.x, ray.y);
	Float NdotRayDirection = glm::dot(m_triangle.normal, ray.direction);
	if (fabs(NdotRayDirection) < ShadowEpsilon) {
		return false;
	}

	Float t = (m_triangle.d - glm::dot(m_triangle.normal, ray.origin)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax) {
		return false;
	}

	Vec3 P = ray.At(t);
	Vec3 C;

	Vec3 vp0 = P - m_triangle.p0;
	C = glm::cross(m_triangle.edge0, vp0);
	if (glm::dot(m_triangle.normal, C) < 0) {
		return false;
	}

	Vec3 vp1 = P - m_triangle.p1;
	C = glm::cross(m_triangle.edge1, vp1);
	if (glm::dot(m_triangle.normal, C) < 0) {
		return false;
	}

	Vec3 vp2 = P - m_triangle.p2;
	C = glm::cross(m_triangle.edge2, vp2);
	if (glm::dot(m_triangle.normal, C) < 0) {
		return false;
	}

	outCollision.triangle = &m_triangle;
	outCollision.distance = t;
	outCollision.backface = NdotRayDirection < 0;
	outCollision.normal = outCollision.backface ? m_triangle.normal : -m_triangle.normal;
	outCollision.position = P;
	outCollision.uv = Vec3(0);
	outCollision.area = m_triangle.area;
	outCollision.dpdu = m_triangle.edge0;

	outCollision.material = m_material;
	outCollision.areaLight = m_areaLight;

	return true;
}

bool TrianglePrimitive::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	stats.m_triangleCheckStatBuffer.Increment(ray.x, ray.y);
	Float NdotRayDirection = glm::dot(m_triangle.normal, ray.direction);
	if (fabs(NdotRayDirection) < ShadowEpsilon) {
		return false;
	}

	Float t = (m_triangle.d - glm::dot(m_triangle.normal, ray.origin)) / NdotRayDirection;
	if (t < ShadowEpsilon || t > tMax) {
		return false;
	}

	Vec3 P = ray.At(t);
	Vec3 C;

	Vec3 vp0 = P - m_triangle.p0;
	C = glm::cross(m_triangle.edge0, vp0);
	if (glm::dot(m_triangle.normal, C) < 0) {
		return false;
	}

	Vec3 vp1 = P - m_triangle.p1;
	C = glm::cross(m_triangle.edge1, vp1);
	if (glm::dot(m_triangle.normal, C) < 0) {
		return false;
	}

	Vec3 vp2 = P - m_triangle.p2;
	C = glm::cross(m_triangle.edge2, vp2);
	if (glm::dot(m_triangle.normal, C) < 0) {
		return false;
	}

	return true;
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
		bvhPrimitives[i] = BVHPrimitive(i, primitives[i]->GetBounds());
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