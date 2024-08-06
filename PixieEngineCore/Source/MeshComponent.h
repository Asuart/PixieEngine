#pragma once
#include "Mesh.h"
#include "Component.h"
#include "Material.h"

class MeshComponent : public Component {
public:
	MeshComponent(Mesh* mesh, const SceneObject* parent);

	const Mesh* GetMesh() const;

protected:
	Mesh* mesh = nullptr;
};
