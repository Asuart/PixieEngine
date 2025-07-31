#pragma once
#include "pch.h"
#include "Ray.h"
#include "Resources/Mesh.h"
#include "Math/Bounds.h"

class MeshBVH {
public:
	struct BVHNode {
		Bounds3f bounds;
		int32_t secondChildOffset = -1;
		uint16_t numObjects = 0;
		uint8_t axis = 0;
		uint8_t leaf = 0;

		BVHNode(int32_t start, int16_t objetsCount, int8_t axis) :
			secondChildOffset(start), numObjects(objetsCount), axis(axis) {}

		BVHNode(glm::vec3 pMin, glm::vec3 pMax, int8_t axis) :
			bounds(pMin, pMax), axis(axis) {}
	};

	struct BVHLeaf {
		Bounds3f bounds;
		int32_t index;

		BVHLeaf(Bounds3f bounds, int32_t index) :
			bounds(bounds), index(index) {}
	};

	MeshBVH(const Mesh& mesh) :
		m_mesh(mesh) {
		for (int32_t i = 0; i < mesh.m_indices.size() / 3; i++) {
			glm::vec3 min = glm::min(
				mesh.m_vertices[mesh.m_indices[i * 3 + 0]].position,
				mesh.m_vertices[mesh.m_indices[i * 3 + 1]].position,
				mesh.m_vertices[mesh.m_indices[i * 3 + 2]].position);
			glm::vec3 max = glm::max(
				mesh.m_vertices[mesh.m_indices[i * 3 + 0]].position,
				mesh.m_vertices[mesh.m_indices[i * 3 + 1]].position,
				mesh.m_vertices[mesh.m_indices[i * 3 + 2]].position);
			m_leaves.push_back({ Bounds3f(min, max), i });
		}
		BuildBVH();
	}

	std::vector<BVHLeaf> m_leaves;
	std::vector<BVHNode> m_nodes;
	Mesh m_mesh;

	inline int32_t MaxComponentIndex(glm::vec3 v) {
		return v.x > v.y ? (v.x > v.z ? 0 : 2) : (v.y > v.z ? 1 : 2);
	}

	void BuildBVH() {
		BuildBVHRecoursive(0, (int32_t)m_leaves.size());
	}

	int32_t BuildBVHRecoursive(int32_t start, int16_t objectsCount) {
		constexpr int32_t MaxObjetsPerNode = 4;
		constexpr int32_t nBuckets = 12;
		constexpr int32_t nSplits = nBuckets - 1;

		glm::vec3 pMin = glm::vec3(INFINITY);
		glm::vec3 pMax = glm::vec3(-INFINITY);
		glm::vec3 pMinNode = glm::vec3(INFINITY);
		glm::vec3 pMaxNode = glm::vec3(-INFINITY);
		for (int32_t i = start; i < start + objectsCount; i++) {
			const Bounds3f& bounds = m_leaves[i].bounds;
			glm::vec3 centroid = bounds.Center();
			pMin = glm::min(pMin, centroid);
			pMax = glm::max(pMax, centroid);
			pMinNode = glm::min(pMinNode, bounds.min);
			pMaxNode = glm::max(pMaxNode, bounds.max);
		}
		int8_t axis = (int8_t)MaxComponentIndex(glm::abs(pMax - pMin));

		std::sort(m_leaves.begin() + start, m_leaves.begin() + start + objectsCount,
			[&](const BVHLeaf& t0, const BVHLeaf& t1) {
				return t0.bounds.Center()[axis] < t1.bounds.Center()[axis];
			});

		std::array<int32_t, nBuckets> bucketCounts;
		std::array<glm::vec3, nBuckets> bucketMins{ glm::vec3(INFINITY) };
		std::array<glm::vec3, nBuckets> bucketMaxs{ glm::vec3(-INFINITY) };

		for (int32_t i = start; i < start + objectsCount; i++) {
			const Bounds3f& bounds = m_leaves[i].bounds;
			int32_t b = int32_t(nBuckets * Bounds3f(pMin, pMax).Offset(bounds.Center())[axis]);
			if (b == nBuckets) {
				b = nBuckets - 1;
			}
			bucketCounts[b]++;
			bucketMins[b] = glm::min(bucketMins[b], bounds.min);
			bucketMaxs[b] = glm::max(bucketMaxs[b], bounds.max);
		}

		std::array<float, nSplits> costs{ 0 };
		int32_t countBelow = 0;
		glm::vec3 pMinBelow = glm::vec3(INFINITY);
		glm::vec3 pMaxBelow = glm::vec3(-INFINITY);
		for (int32_t i = 0; i < nSplits; i++) {
			pMinBelow = glm::min(pMinBelow, bucketMins[i]);
			pMaxBelow = glm::max(pMaxBelow, bucketMaxs[i]);
			countBelow += bucketCounts[i];
			costs[i] += countBelow * Bounds3f(pMinBelow, pMaxBelow).Area();
		}

		int32_t countAbove = 0;
		glm::vec3 pMinAbove = glm::vec3(INFINITY);
		glm::vec3 pMaxAbove = glm::vec3(-INFINITY);
		for (int32_t i = nSplits; i >= 1; i--) {
			pMinAbove = glm::min(pMinAbove, bucketMins[i]);
			pMaxAbove = glm::max(pMaxAbove, bucketMaxs[i]);
			countAbove += bucketCounts[i];
			costs[i - 1] += countAbove * Bounds3f(pMinAbove, pMaxAbove).Area();
		}

		int32_t minCostSplitBucket = -1;
		float minCost = INFINITY;
		for (int32_t i = 0; i < nSplits; i++) {
			if (costs[i] < minCost) {
				minCost = costs[i];
				minCostSplitBucket = i;
			}
		}

		float leafCost = (float)objectsCount;
		minCost = 1.0f / 2.0f + minCost / Bounds3f(pMinNode, pMaxNode).Area();

		if (objectsCount <= MaxObjetsPerNode || minCost > leafCost) {
			m_nodes.push_back(BVHNode(start, objectsCount, axis));
			m_nodes.back().bounds.min = pMinNode;
			m_nodes.back().bounds.max = pMaxNode;
			return (int32_t)m_nodes.size() - 1;
		}

		auto midIter = std::partition(m_leaves.begin() + start, m_leaves.begin() + start + objectsCount,
			[=](const BVHLeaf& leaf) {
				int32_t b = int32_t(nBuckets * Bounds3f(pMin, pMax).Offset(leaf.bounds.Center())[axis]);
				if (b == nBuckets) {
					b = nBuckets - 1;
				}
				return b <= minCostSplitBucket;
			});
		int32_t mid = int32_t(midIter - m_leaves.begin());

		int32_t nodeIndex = (int32_t)m_nodes.size();
		m_nodes.push_back(BVHNode(pMinNode, pMaxNode, axis));

		BuildBVHRecoursive(start, mid - start);
		int32_t secondChild = BuildBVHRecoursive(mid, objectsCount - (mid - start));

		m_nodes[nodeIndex].secondChildOffset = secondChild;

		return nodeIndex;
	}


	int32_t CountIntersections(const Ray& ray) const {
		if (m_nodes.size() == 0) {
			return 0;
		}
		int32_t intersectionsCount = 0;
		std::array<int32_t, 64> objectsStack;
		int32_t objectsStackSize = 0;
		if (IsAABBIntersected(m_nodes[0].bounds, ray)) {
			objectsStack[objectsStackSize++] = 0;
		}
		while (objectsStackSize > 0) {
			objectsStackSize--;
			int32_t objectIndex = objectsStack[objectsStackSize];
			const BVHNode& objectNode = m_nodes[objectIndex];
			if (objectNode.numObjects == 0) {
				if (IsAABBIntersected(m_nodes[objectIndex + 1].bounds, ray)) {
					objectsStack[objectsStackSize++] = objectIndex + 1;
				}
				if (IsAABBIntersected(m_nodes[objectNode.secondChildOffset].bounds, ray)) {
					objectsStack[objectsStackSize++] = objectNode.secondChildOffset;
				}
			}
			else {
				for (int32_t i = 0; i < objectNode.numObjects; i++) {
					const BVHLeaf& leaf = m_leaves[objectNode.secondChildOffset + i];
					if (IsAABBIntersected(leaf.bounds, ray)) {
						if (IsTriangleIntersected(
							m_mesh.m_vertices[m_mesh.m_indices[leaf.index * 3 + 0]].position,
							m_mesh.m_vertices[m_mesh.m_indices[leaf.index * 3 + 1]].position,
							m_mesh.m_vertices[m_mesh.m_indices[leaf.index * 3 + 2]].position,
							ray)) {
							intersectionsCount++;
						}
					}
				}
			}
		}
		return intersectionsCount;
	}

	bool IsTriangleIntersected(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, const Ray& ray) const {
		const float epsilon = 0.001f;

		glm::vec3 edge1 = p1 - p0;
		glm::vec3 edge2 = p2 - p0;
		glm::vec3 ray_cross_e2 = glm::cross(ray.direction, edge2);
		float det = glm::dot(edge1, ray_cross_e2);

		if (det > -epsilon && det < epsilon) {
			return false;
		}

		float inv_det = 1.0f / det;
		glm::vec3 s = ray.origin - p0;
		float u = inv_det * dot(s, ray_cross_e2);

		if (u < 0 || u > 1) {
			return false;
		}

		glm::vec3 s_cross_e1 = cross(s, edge1);
		float v = inv_det * dot(ray.direction, s_cross_e1);

		if (v < 0 || u + v > 1) {
			return false;
		}

		float t = inv_det * dot(edge2, s_cross_e1);

		if (t < epsilon) {
			return false;
		}

		return true;
	}

	bool IsAABBIntersected(const Bounds3f& aabb, const Ray& ray) const {
		float t0 = 0, t1 = INFINITY;
		for (int32_t i = 0; i < 3; ++i) {
			float tNear = (aabb.min[i] - ray.origin[i]) * ray.inverseDirection[i];
			float tFar = (aabb.max[i] - ray.origin[i]) * ray.inverseDirection[i];

			if (tNear > tFar) {
				std::swap(tNear, tFar);
			}

			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1) {
				return false;
			}
		}
		return true;
	}
};
