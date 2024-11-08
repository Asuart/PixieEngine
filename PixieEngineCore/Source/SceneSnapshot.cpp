#include "pch.h"
#include "SceneSnapshot.h"
#include "Scene.h"

SceneSnapshot::SceneSnapshot(Scene* scene, uint32_t maxPrimitivesPerLeaf) {
	uint64_t trianglesCount = 0, invalidTrianglesCount = 0;
	std::vector<SceneObject*> flatObjects = scene->FindObjectsWithComponent(ComponentType::Mesh);

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
		for (size_t i = 0; i < mesh->m_indices.size() / 3; i++) {
			Triangle* triangle = new Triangle(
				mesh->m_vertices[mesh->m_indices[i * 3 + 0]],
				mesh->m_vertices[mesh->m_indices[i * 3 + 1]],
				mesh->m_vertices[mesh->m_indices[i * 3 + 2]]
			);

			trianglesCount++;
			if (!triangle->area || isnan(triangle->normal)) {
				invalidTrianglesCount++;
				continue;
			}

			DiffuseAreaLight* areaLight = nullptr;
			if (material->m_emissionStrength) {
				areaLight = new DiffuseAreaLight(triangle, Transform(), material);
				m_areaLights.push_back(areaLight);
			}
			shapePrimitives.push_back(new ShapePrimitive(triangle, material, areaLight));
		}
		agregatePrimitives.push_back(new BVHAggregate(shapePrimitives, maxPrimitivesPerLeaf));
	}

	m_rootPrimitive = new BVHAggregate(agregatePrimitives, maxPrimitivesPerLeaf);

	std::cout << "Scene triangles count: " << trianglesCount << ", invalid triangles count: " << invalidTrianglesCount << "\n";
}

SceneSnapshot::~SceneSnapshot() {
	if (m_rootPrimitive) delete m_rootPrimitive;
	for (size_t i = 0; i < m_areaLights.size(); i++) {
		delete m_areaLights[i];
	}
}

const Primitive* SceneSnapshot::GetRootPrimitive() const {
	return m_rootPrimitive;
}

std::vector<DiffuseAreaLight*>& SceneSnapshot::GetAreaLights() {
	return m_areaLights;
}

std::vector<Light*>& SceneSnapshot::GetInfiniteLights() {
	return m_infiniteLights;
}

Bounds3f SceneSnapshot::GetBounds() const {
	if (m_rootPrimitive) {
		return m_rootPrimitive->GetBounds();
	}
	else {
		return Bounds3f();
	}
}
