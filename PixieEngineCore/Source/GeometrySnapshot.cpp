#include "pch.h"
#include "GeometrySnapshot.h"

GeometrySnapshot::GeometrySnapshot(const std::vector<SceneObject*>& flatObjects, uint32_t maxPrimitivesPerLeaf) {
	uint64_t trianglesCount = 0, invalidTrianglesCount = 0;


	std::vector<Primitive*> agregatePrimitives;
	for (size_t sceneObjectIndex = 0; sceneObjectIndex < flatObjects.size(); sceneObjectIndex++) {
		SceneObject* object = flatObjects[sceneObjectIndex];

		MeshComponent* meshComopnent = object->GetComponent<MeshComponent>();
		MaterialComponent* materialComponent = object->GetComponent<MaterialComponent>();
		if (!meshComopnent || !materialComponent) continue;

		const Mesh* mesh = meshComopnent->GetMesh();
		const Material* material = materialComponent->GetMaterial();
		if (!mesh || !material) continue;

		std::vector<Primitive*> shapePrimitives;
		for (size_t i = 0; i < mesh->indices.size(); i += 3) {
			TriangleCache triangle = TriangleCache(
				mesh->vertices[mesh->indices[i + 0]],
				mesh->vertices[mesh->indices[i + 1]],
				mesh->vertices[mesh->indices[i + 2]]
			);

			trianglesCount++;
			if (isnan(triangle.normal)) {
				invalidTrianglesCount++;
				continue;
			}

			AreaLight* areaLight = nullptr;
			if (material->m_emissionStrength) {
				areaLight = new DiffuseAreaLight(triangle, material);
				m_areaLights.push_back(areaLight);
			}
			shapePrimitives.push_back(new TrianglePrimitive(triangle, material, areaLight));
		}
		agregatePrimitives.push_back(new BVHAggregate(shapePrimitives, maxPrimitivesPerLeaf));
	}

	m_rootPrimitive = new BVHAggregate(agregatePrimitives, maxPrimitivesPerLeaf);

	std::cout << "Scene triangles count: " << trianglesCount << ", invalid triangles count: " << invalidTrianglesCount << "\n";
}

GeometrySnapshot::~GeometrySnapshot() {
	if (m_rootPrimitive) delete m_rootPrimitive;
	for (size_t i = 0; i < m_areaLights.size(); i++) {
		delete m_areaLights[i];
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