#pragma once
#include "Mesh.h"
#include "Component.h"
#include "Material.h"

class MeshComponent : public Component {
public:
	MeshComponent(Mesh* mesh, SceneObject* parent);

	const Mesh* GetMesh() const;
	Mesh* GetMesh();
	void Draw() const;
	void UploadMesh();

protected:
	Mesh* m_mesh;
};
