#include "GeometrySnapshot.h"

GeometrySnapshot::GeometrySnapshot(const std::vector<SceneObject*>& flatObjects, uint32_t maxPrimitivesPerLeaf, bool cacheTriangles) {
	uint64_t trianglesCount = 0, invalidTrianglesCount = 0;

	std::vector<Primitive*> shapePrimitives;
	for (size_t sceneObjectIndex = 0; sceneObjectIndex < flatObjects.size(); sceneObjectIndex++) {
		SceneObject* object = flatObjects[sceneObjectIndex];

		MeshComponent* meshComopnent = object->GetComponent<MeshComponent>();
		MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>();
		if (!meshComopnent || !materialComponent) continue;

		const Mesh* mesh = meshComopnent->GetMesh();
		const Material* material = materialComponent->GetMaterial();
		if (!mesh || !material) continue;

		for (size_t i = 0; i < mesh->indices.size(); i += 3) {
			Vec3 p0 = mesh->vertices[mesh->indices[i + 0]].position;
			Vec3 p1 = mesh->vertices[mesh->indices[i + 1]].position;
			Vec3 p2 = mesh->vertices[mesh->indices[i + 2]].position;

			Shape* triangle = nullptr;
			if (cacheTriangles) {
				triangle = new CachedTriangle(p0, p1, p2);
			}
			else {
				triangle = new Triangle(p0, p1, p2);
			}

			trianglesCount++;
			if (!triangle->IsValid()) {
				delete triangle;
				invalidTrianglesCount++;
				continue;
			}

			m_shapes.push_back(triangle);
			AreaLight* areaLight = nullptr;
			if (material->emission != glm::vec3(0.0f)) {
				areaLight = new DiffuseAreaLight(triangle, material->emission);
				m_areaLights.push_back(areaLight);
			}
			shapePrimitives.push_back(new ShapePrimitive(triangle, material, areaLight));
		}
	}

	m_rootPrimitive = new BVHAggregate(shapePrimitives, maxPrimitivesPerLeaf);

	std::cout << "Scene triangles count: " << trianglesCount << ", invalid triangles count: " << invalidTrianglesCount << "\n";
}

GeometrySnapshot::~GeometrySnapshot() {
	if(m_rootPrimitive) delete m_rootPrimitive;
	for (size_t i = 0; i < m_shapes.size(); i++) {
		delete m_shapes[i];
	}
}

bool GeometrySnapshot::Intersect(const Ray& ray, SurfaceInteraction& outCollision, RayTracingStatistics& stats, Float tMax) const {
	return m_rootPrimitive->Intersect(ray, outCollision, stats, tMax);
}

bool GeometrySnapshot::IntersectP(const Ray& ray, RayTracingStatistics& stats, Float tMax) const {
	return m_rootPrimitive->IntersectP(ray, stats, tMax);
}

std::vector<AreaLight*>& GeometrySnapshot::GetAreaLights() {
	return m_areaLights;
}