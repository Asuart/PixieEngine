#pragma once
#include "Mesh.h"
#include "Component.h"
#include "Material.h"

class MeshComponent : public Component {
public:
	MeshComponent(Mesh* mesh, const SceneObject* parent);

protected:
	Mesh* mesh = nullptr;
};
