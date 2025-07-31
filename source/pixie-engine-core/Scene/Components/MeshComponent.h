#pragma once
#include "Resources/MeshHandle.h"
#include "Component.h"

class MeshComponent : public Component {
public:
	MeshComponent(SceneObject* parent, MeshHandle mesh);

	MeshHandle GetMesh() const;
	void SetMesh(MeshHandle mesh);
	void Draw() const;
	uint32_t GetIndicesCount() const;

protected:
	MeshHandle m_mesh;
};
