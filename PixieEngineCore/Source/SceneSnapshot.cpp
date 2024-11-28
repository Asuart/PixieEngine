#include "pch.h"
#include "SceneSnapshot.h"
#include "Scene.h"
#include "ResourceManager.h"

SceneSnapshot::SceneSnapshot(Scene* scene) {
	m_invalidTrianglesCount = 0;
	std::vector<SceneObject*> flatObjects = scene->FindObjectsWithComponent(ComponentType::Mesh);
	std::vector<ObjectCache> objects;

	int32_t maxTrianglesCount = 0;
	for (size_t i = 0; i < flatObjects.size(); i++) {
		maxTrianglesCount += (int32_t)flatObjects[i]->GetComponent<MeshComponent>()->GetMesh()->m_indices.size() / 3;
	}
	m_triangles.reserve(maxTrianglesCount);

	for (size_t sceneObjectIndex = 0; sceneObjectIndex < flatObjects.size(); sceneObjectIndex++) {
		SceneObject* object = flatObjects[sceneObjectIndex];
		MeshComponent* meshComopnent = object->GetComponent<MeshComponent>();
		MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>();
		if (!meshComopnent || !materialComponent) {
			continue;
		}

		Mesh* mesh = meshComopnent->GetMesh();
		Material* material = materialComponent->GetMaterial();
		if (!mesh || !material) {
			continue;
		}

		int32_t nodeIndex = BuildMeshBVH(mesh, material);
		if (nodeIndex == -1) {
			continue;
		}
		objects.push_back(ObjectCache(ObjectType::Mesh, nodeIndex, Bounds3f(m_nodes[nodeIndex].pMin, m_nodes[nodeIndex].pMax)));
	}

	std::vector<SceneObject*> spheres = scene->FindObjectsWithComponent(ComponentType::Sphere);
	for (int32_t i = 0; i < spheres.size(); i++) {
		SphereComponent* sphereComponent = spheres[i]->GetComponent<SphereComponent>();
		MaterialComponent* materialComponent = spheres[i]->GetComponent<MaterialComponent>();
		if (!sphereComponent || !materialComponent) {
			continue;
		}
		m_spheres.push_back(Sphere(ResourceManager::GetMaterialIndex(materialComponent->GetMaterial()), spheres[i]->GetTransform(), sphereComponent->GetRadius()));
		objects.push_back(ObjectCache(ObjectType::Sphere, (int32_t)m_spheres.size() - 1, m_spheres.back().Bounds()));
	}

	Skybox* skybox = scene->GetSkybox();
	if (skybox) {
		if (SolidColorSkybox* solidColorSkybox = dynamic_cast<SolidColorSkybox*>(skybox); solidColorSkybox) {
			UniformInfiniteLight* light = new UniformInfiniteLight(solidColorSkybox->m_color, 1.0f, Transform());
			m_infiniteLights.push_back(light);
			m_lights.push_back(light);
		}
		else if (GradientColorSkybox* gradientColorSkybox = dynamic_cast<GradientColorSkybox*>(skybox); gradientColorSkybox) {
			GradientInfiniteLight* light = new GradientInfiniteLight(gradientColorSkybox->m_topColor, gradientColorSkybox->m_bottomColor, 1.0f, Transform());
			m_infiniteLights.push_back(light);
			m_lights.push_back(light);
		}
	}

	std::vector<SceneObject*> directionalLights = scene->FindObjectsWithComponent(ComponentType::DirectionalLight);
	for (int32_t i = 0; i < directionalLights.size(); i++) {
		DirectionalLightComponent* lightComponent = directionalLights[i]->GetComponent<DirectionalLightComponent>();
		DirectionalLight* light = new DirectionalLight(lightComponent->GetColor(), lightComponent->GetStrength(), directionalLights[i]->GetTransform());
		m_infiniteLights.push_back(light);
		m_lights.push_back(light);
	}

	std::vector<SceneObject*> pointLights = scene->FindObjectsWithComponent(ComponentType::PointLight);
	for (int32_t i = 0; i < pointLights.size(); i++) {
		PointLightComponent* lightComponent = pointLights[i]->GetComponent<PointLightComponent>();
		PointLight* light = new PointLight(lightComponent->GetColor(), lightComponent->GetStrength(), pointLights[i]->GetTransform());
		m_infiniteLights.push_back(light);
		m_lights.push_back(light);
	}

	std::vector<SceneObject*> cameras = scene->FindObjectsWithComponent(ComponentType::Camera);
	for (int32_t i = 0; i < cameras.size(); i++) {
		CameraComponent* cameraComponent = cameras[i]->GetComponent<CameraComponent>();
		Camera camera = cameraComponent->GetCamera();
		camera.GetTransform() = cameras[i]->GetTransform();
		m_cameras.push_back(camera);
	}

	std::vector<Material>& materials = ResourceManager::GetMaterials();
	m_materials.resize(materials.size());
	for (int32_t i = 0; i < materials.size(); i++) {
		m_materials[i] = new ProceduralMaterial(materials[i]);
	}

	BuildObjectsBVHRecoursive(objects, 0, (int32_t)objects.size());
}

int32_t SceneSnapshot::BuildMeshBVH(Mesh* mesh, Material* material) {
	int32_t materialIndex = ResourceManager::GetMaterialIndex(material);

	int32_t startIndex = (int32_t)m_triangles.size();
	for (size_t i = 0; i < mesh->m_indices.size() / 3; i++) {
		Triangle triangle = Triangle(materialIndex, 
			mesh->m_vertices[mesh->m_indices[i * 3 + 0]],
			mesh->m_vertices[mesh->m_indices[i * 3 + 1]],
			mesh->m_vertices[mesh->m_indices[i * 3 + 2]]
		);

		if (!triangle.Area() || isnan(triangle.normal)) {
			m_invalidTrianglesCount++;
			continue;
		}
		m_triangles.push_back(triangle);

		DiffuseAreaLight* areaLight = nullptr;
		if (material->m_emissionStrength) {
			areaLight = new DiffuseAreaLight(&m_triangles.back(), Transform(), materialIndex);
			m_triangles[m_triangles.size() - 1].m_lightIndex = (int32_t)m_areaLights.size();
			m_areaLights.push_back(areaLight);
			m_lights.push_back(areaLight);
		}
	}
	int32_t trianglesCount = (int32_t)m_triangles.size() - startIndex;
	if (trianglesCount == 0) {
		return -1;
	}
	return BuildBVHRecoursive(startIndex, trianglesCount);
}

int32_t SceneSnapshot::BuildObjectsBVHRecoursive(std::vector<ObjectCache>& cache, int32_t start, int32_t objectsCount) {
	constexpr int32_t nBuckets = 12;
	constexpr int32_t nSplits = nBuckets - 1;

	Vec3 pMin = Vec3(Infinity);
	Vec3 pMax = Vec3(-Infinity);
	Vec3 pMinNode = Vec3(Infinity);
	Vec3 pMaxNode = Vec3(-Infinity);
	for (int32_t i = start; i < start + objectsCount; i++) {
		const ObjectCache& c = cache[i];
		Vec3 centroid = c.bounds.Center();
		pMin = glm::min(pMin, centroid);
		pMax = glm::max(pMax, centroid);
		pMinNode = glm::min(pMinNode, c.bounds.min);
		pMaxNode = glm::max(pMaxNode, c.bounds.max);
	}
	int32_t axis = MaxComponentIndex(glm::abs(pMax - pMin));

	std::sort(cache.begin() + start, cache.begin() + start + objectsCount,
		[&](const ObjectCache& t0, const ObjectCache& t1) {
			return t0.bounds.Center()[axis] < t1.bounds.Center()[axis];
		});

	std::array<int32_t, nBuckets> bucketCounts;
	std::array<Vec3, nBuckets> bucketMins{ Vec3(Infinity) };
	std::array<Vec3, nBuckets> bucketMaxs{ Vec3(-Infinity) };

	for (int32_t i = start; i < start + objectsCount; i++) {
		const ObjectCache& c = cache[i];
		int32_t b = int32_t(nBuckets * Bounds3f(pMin, pMax).Offset(c.bounds.Center())[axis]);
		if (b == nBuckets) {
			b = nBuckets - 1;
		}
		bucketCounts[b]++;
		bucketMins[b] = glm::min(bucketMins[b], c.bounds.min);
		bucketMaxs[b] = glm::max(bucketMaxs[b], c.bounds.max);
	}

	std::array<Float, nSplits> costs{ 0 };
	int32_t countBelow = 0;
	Vec3 pMinBelow = Vec3(Infinity);
	Vec3 pMaxBelow = Vec3(-Infinity);
	for (int32_t i = 0; i < nSplits; i++) {
		pMinBelow = glm::min(pMinBelow, bucketMins[i]);
		pMaxBelow = glm::max(pMaxBelow, bucketMaxs[i]);
		countBelow += bucketCounts[i];
		costs[i] += countBelow * Bounds3f(pMinBelow, pMaxBelow).Area();
	}

	int32_t countAbove = 0;
	Vec3 pMinAbove = Vec3(Infinity);
	Vec3 pMaxAbove = Vec3(-Infinity);
	for (int32_t i = nSplits; i >= 1; i--) {
		pMinAbove = glm::min(pMinAbove, bucketMins[i]);
		pMaxAbove = glm::max(pMaxAbove, bucketMaxs[i]);
		countAbove += bucketCounts[i];
		costs[i - 1] += countAbove * Bounds3f(pMinAbove, pMaxAbove).Area();
	}

	int32_t minCostSplitBucket = -1;
	Float minCost = Infinity;
	for (int32_t i = 0; i < nSplits; i++) {
		if (costs[i] < minCost) {
			minCost = costs[i];
			minCostSplitBucket = i;
		}
	}

	Float leafCost = (Float)objectsCount;
	minCost = 1.0f / 2.0f + minCost / Bounds3f(pMinNode, pMaxNode).Area();

	if (objectsCount <= ObjectBVHNodeChildrenCount || minCost > leafCost) {
		m_objects.push_back(ObjectBVHNode(Transform(), pMinNode, pMaxNode));
		for (int32_t childIndex = 0; childIndex < ObjectBVHNodeChildrenCount && childIndex < objectsCount; childIndex++) {
			m_objects.back().childrenTypes[childIndex] = cache[start + childIndex].type;
			m_objects.back().childrenIndexes[childIndex] = cache[start + childIndex].index;
		}
		return (int32_t)m_objects.size() - 1;
	}

	auto midIter = std::partition(cache.begin() + start, cache.begin() + start + objectsCount,
		[=](const ObjectCache& c) {
			int32_t b = int32_t(nBuckets * Bounds3f(pMin, pMax).Offset(c.bounds.Center())[axis]);
			if (b == nBuckets) {
				b = nBuckets - 1;
			}
			return b <= minCostSplitBucket;
		});
	int32_t mid = int32_t(midIter - cache.begin());

	int32_t nodeIndex = (int32_t)m_objects.size();
	m_objects.push_back(ObjectBVHNode(Transform(), pMinNode, pMaxNode));

	int32_t firstChild = BuildObjectsBVHRecoursive(cache, start, mid - start);
	int32_t secondChild = BuildObjectsBVHRecoursive(cache, mid, objectsCount - (mid - start));

	m_objects[nodeIndex].childrenIndexes[0] = firstChild;
	m_objects[nodeIndex].childrenTypes[0] = ObjectType::Node;
	m_objects[nodeIndex].childrenIndexes[1] = secondChild;
	m_objects[nodeIndex].childrenTypes[1] = ObjectType::Node;

	return nodeIndex;
}

int32_t SceneSnapshot::BuildBVHRecoursive(int32_t start, int32_t trianglesCount) {
	constexpr int32_t MaxTrianglesPerNode = 4;
	constexpr int32_t nBuckets = 12;
	constexpr int32_t nSplits = nBuckets - 1;

	Vec3 pMin = Vec3(Infinity);
	Vec3 pMax = Vec3(-Infinity);
	Vec3 pMinNode = Vec3(Infinity);
	Vec3 pMaxNode = Vec3(-Infinity);
	for (int32_t i = start; i < start + trianglesCount; i++) {
		const Triangle& tri = m_triangles[i];
		Bounds3f bounds = tri.Bounds();
		Vec3 centroid = bounds.Center();
		pMin = glm::min(pMin, centroid);
		pMax = glm::max(pMax, centroid);
		pMinNode = glm::min(pMinNode, bounds.min);
		pMaxNode = glm::max(pMaxNode, bounds.max);
	}
	int32_t axis = MaxComponentIndex(glm::abs(pMax - pMin));

	std::sort(m_triangles.begin() + start, m_triangles.begin() + start + trianglesCount,
		[&](const Triangle& t0, const Triangle& t1) {
			return t0.Bounds().Center()[axis] < t1.Bounds().Center()[axis];
		});

	std::array<int32_t, nBuckets> bucketCounts;
	std::array<Vec3, nBuckets> bucketMins{ Vec3(Infinity) };
	std::array<Vec3, nBuckets> bucketMaxs{ Vec3(-Infinity) };

	for (int32_t i = start; i < start + trianglesCount; i++) {
		const Triangle& tri = m_triangles[i];
		Bounds3f bounds = tri.Bounds();
		int32_t b = int32_t(nBuckets * Bounds3f(pMin, pMax).Offset(bounds.Center())[axis]);
		if (b == nBuckets) {
			b = nBuckets - 1;
		}
		bucketCounts[b]++;
		bucketMins[b] = glm::min(bucketMins[b], bounds.min);
		bucketMaxs[b] = glm::max(bucketMaxs[b], bounds.max);
	}

	std::array<Float, nSplits> costs{ 0 };
	int32_t countBelow = 0;
	Vec3 pMinBelow = Vec3(Infinity);
	Vec3 pMaxBelow = Vec3(-Infinity);
	for (int32_t i = 0; i < nSplits; i++) {
		pMinBelow = glm::min(pMinBelow, bucketMins[i]);
		pMaxBelow = glm::max(pMaxBelow, bucketMaxs[i]);
		countBelow += bucketCounts[i];
		costs[i] += countBelow * Bounds3f(pMinBelow, pMaxBelow).Area();
	}

	int32_t countAbove = 0;
	Vec3 pMinAbove = Vec3(Infinity);
	Vec3 pMaxAbove = Vec3(-Infinity);
	for (int32_t i = nSplits; i >= 1; i--) {
		pMinAbove = glm::min(pMinAbove, bucketMins[i]);
		pMaxAbove = glm::max(pMaxAbove, bucketMaxs[i]);
		countAbove += bucketCounts[i];
		costs[i - 1] += countAbove * Bounds3f(pMinAbove, pMaxAbove).Area();
	}

	int32_t minCostSplitBucket = -1;
	Float minCost = Infinity;
	for (int32_t i = 0; i < nSplits; i++) {
		if (costs[i] < minCost) {
			minCost = costs[i];
			minCostSplitBucket = i;
		}
	}

	Float leafCost = (Float)trianglesCount;
	minCost = 1.0f / 2.0f + minCost / Bounds3f(pMinNode, pMaxNode).Area();

	if (trianglesCount <= MaxTrianglesPerNode || minCost > leafCost) {
		m_nodes.push_back(BVHNode(start, trianglesCount, axis));
		m_nodes.back().pMin = pMinNode;
		m_nodes.back().pMax = pMaxNode;
		return (int32_t)m_nodes.size() - 1;
	}

	auto midIter = std::partition(m_triangles.begin() + start, m_triangles.begin() + start + trianglesCount,
		[=](const Triangle& tri) {
			int32_t b = int32_t(nBuckets * Bounds3f(pMin, pMax).Offset(tri.Bounds().Center())[axis]);
			if (b == nBuckets) {
				b = nBuckets - 1;
			}
			return b <= minCostSplitBucket;
		});
	int32_t mid = int32_t(midIter - m_triangles.begin());

	int32_t nodeIndex = (int32_t)m_nodes.size();
	m_nodes.push_back(BVHNode(pMinNode, pMaxNode, axis));

	int32_t firstChild = BuildBVHRecoursive(start, mid - start);
	int32_t secondChild = BuildBVHRecoursive(mid, trianglesCount - (mid - start));

	m_nodes[nodeIndex].childOffset = secondChild;

	return nodeIndex;
}

SceneSnapshot::~SceneSnapshot() {
	for (size_t i = 0; i < m_areaLights.size(); i++) {
		delete m_areaLights[i];
	}
}

std::vector<BVHNode>& SceneSnapshot::GetNodes() {
	return m_nodes;
}

std::vector<Light*>& SceneSnapshot::GetLights() {
	return m_lights;
}

std::vector<DiffuseAreaLight*>& SceneSnapshot::GetAreaLights() {
	return m_areaLights;
}

DiffuseAreaLight* SceneSnapshot::GetAreaLight(int32_t index) {
	return m_areaLights[index];
}

std::vector<Light*>& SceneSnapshot::GetInfiniteLights() {
	return m_infiniteLights;
}

std::vector<Triangle>& SceneSnapshot::GetTriangles() {
	return m_triangles;
}

std::vector<Camera>& SceneSnapshot::GetCameras() {
	return m_cameras;
}

Bounds3f SceneSnapshot::GetBounds() const {
	return m_nodes.size() > 0 ? Bounds3f(m_nodes[0].pMin, m_nodes[0].pMax) : Bounds3f();
}

uint32_t SceneSnapshot::GetTrianglesCount() {
	return (int32_t)m_triangles.size();
}

uint32_t SceneSnapshot::GetInvalidTrianglesCount() {
	return m_invalidTrianglesCount;
}

uint32_t SceneSnapshot::GetNodesCount() {
	return (int32_t)m_nodes.size();
}

ProceduralMaterial* SceneSnapshot::GetMaterial(int32_t index) {
	return m_materials[index];
}

std::optional<ShapeIntersection> SceneSnapshot::Intersect(const Ray& ray, int32_t* boxChecks, int32_t* shapeChecks, Float tMax) {
	std::optional<ShapeIntersection> intersection = {};
	if (m_objects.size() == 0) {
		return intersection;
	}
	std::array<int32_t, 64> objectsStack;
	int32_t objectsStackSize = 0;
	(*boxChecks)++;
	if (m_objects[0].IsIntersected(ray, tMax)) {
		objectsStack[objectsStackSize++] = 0;
	}
	while (objectsStackSize > 0) {
		objectsStackSize--;
		ObjectBVHNode& objectNode = m_objects[objectsStack[objectsStackSize]];
		for (int32_t childIndex = 0; childIndex < ObjectBVHNodeChildrenCount; childIndex++) {
			int32_t objectIndex = objectNode.childrenIndexes[childIndex];
			if (objectIndex >= 0) {
				switch (objectNode.childrenTypes[childIndex]) {
				case ObjectType::Node: {
					(*boxChecks)++;
					if (m_objects[objectIndex].IsIntersected(ray, tMax)) {
						objectsStack[objectsStackSize++] = objectIndex;
					}
					break;
				}
				case ObjectType::Sphere: {
					(*shapeChecks)++;
					std::optional<ShapeIntersection> si = m_spheres[objectIndex].Intersect(ray, tMax);
					if (si) {
						tMax = si->tHit;
						intersection = si;
					}
					break;
				}
				case ObjectType::Mesh: {
					std::array<int32_t, 64> nodesStack;
					int32_t nodesStackSize = 0;
					(*boxChecks)++;
					if (m_nodes[objectIndex].IsIntersected(ray, tMax)) {
						nodesStack[nodesStackSize++] = objectIndex;
					}
					while (nodesStackSize > 0) {
						nodesStackSize--;
						int32_t nodeIndex = nodesStack[nodesStackSize];
						const BVHNode& node = m_nodes[nodeIndex];
						if (node.nTriangles > 0) {
							for (int32_t i = 0; i < node.nTriangles; i++) {
								(*shapeChecks)++;
								std::optional<ShapeIntersection> si = m_triangles[node.childOffset + i].Intersect(ray, tMax);
								if (si) {
									tMax = si->tHit;
									intersection = si;
								}
							}
							continue;
						}
						int32_t firstChild = nodeIndex + 1;
						int32_t secondChild = node.childOffset;
						Float firstDistance = m_nodes[firstChild].IsIntersected(ray, tMax);
						if (firstDistance != Infinity) {
							nodesStack[nodesStackSize++] = firstChild;
						}
						Float secondDistance = m_nodes[secondChild].IsIntersected(ray, tMax);
						if (secondDistance != Infinity) {
							nodesStack[nodesStackSize++] = secondChild;
						}
						(*boxChecks) += 2;
					}
				}
				}
			}
		}
		
	}
	return intersection;
}

bool SceneSnapshot::IsIntersected(const Ray& ray, int32_t* boxChecks, int32_t* shapeChecks, Float tMax) {
	for (int32_t i = 0; i < m_spheres.size(); i++) {
		(*shapeChecks)++;
		if (m_spheres[i].IsIntersected(ray, tMax)) {
			return true;
		}
	}
	if (m_triangles.size() == 0) {
		return false;
	}
	std::array<int32_t, 64> nodesStack;
	int32_t stackSize = 0;
	(*boxChecks)++;
	Float rootDistance = m_nodes[0].IsIntersected(ray, tMax);
	if (rootDistance != Infinity) {
		nodesStack[stackSize] = 0;
		stackSize++;
	}
	while (stackSize > 0) {
		stackSize--;
		int32_t nodeIndex = nodesStack[stackSize];
		const BVHNode& node = m_nodes[nodeIndex];
		if (node.nTriangles > 0) {
			for (int32_t i = 0; i < node.nTriangles; i++) {
				(*shapeChecks)++;
				if (m_triangles[node.childOffset + i].IsIntersected(ray, tMax)) {
					return true;
				}
			}
			continue;
		}
		int32_t firstChild = nodeIndex + 1;
		int32_t secondChild = node.childOffset;
		Float firstDistance = m_nodes[firstChild].IsIntersected(ray, tMax);
		if (firstDistance != Infinity) {
			nodesStack[stackSize] = firstChild;
			stackSize++;
		}
		Float secondDistance = m_nodes[secondChild].IsIntersected(ray, tMax);
		if (secondDistance != Infinity) {
			nodesStack[stackSize] = secondChild;
			stackSize++;
		}
		(*boxChecks) += 2;
	}
	return false;
}
