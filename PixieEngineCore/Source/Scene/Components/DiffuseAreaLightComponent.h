#pragma once
#include "Component.h"
#include "MeshComponent.h"

class DiffuseAreaLightComponent : public Component {
public:
public:
	DiffuseAreaLightComponent(SceneObject* parent, MeshHandle meshHandle, Mesh mesh, glm::vec3 color, float strength);

	void OnStart() override;

	glm::vec3 GetEmission() const;
	glm::vec3 GetColor() const;
	void SetColor(glm::vec3 color);
	float GetStrength() const;
	void SetStrength(float strength);
	MeshHandle GetMeshHandle() const;
	void SetMeshHandle(MeshHandle mesh);
	const Mesh& GetMesh() const;
	void SetMesh(const Mesh& mesh);

protected:
	MeshHandle m_meshHandle;
	Mesh m_mesh;
	glm::vec3 m_color;
	float m_strength;
};
