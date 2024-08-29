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
			TriangleCache* triangle = new TriangleCache(
				mesh->vertices[mesh->indices[i + 0]],
				mesh->vertices[mesh->indices[i + 1]],
				mesh->vertices[mesh->indices[i + 2]]
			);

			trianglesCount++;
			if (!triangle->area || isnan(triangle->normal)) {
				invalidTrianglesCount++;
				continue;
			}

			DiffuseAreaLight* areaLight = nullptr;
			if (material->m_emissionStrength) {
				areaLight = new DiffuseAreaLight(triangle, material);
				m_areaLights.push_back(areaLight);
			}
			shapePrimitives.push_back(new ShapePrimitive(triangle, material, areaLight));
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

std::optional<ShapeIntersection> GeometrySnapshot::Intersect(const Ray& ray, Float tMax) const {
	return m_rootPrimitive->Intersect(ray, tMax);
}

bool GeometrySnapshot::IntersectP(const Ray& ray, Float tMax) const {
	return m_rootPrimitive->IntersectP(ray, tMax);
}

std::vector<DiffuseAreaLight*>& GeometrySnapshot::GetAreaLights() {
	return m_areaLights;
}

Bounds3f GeometrySnapshot::GetBounds() {
	return m_rootPrimitive->GetBounds();
}
