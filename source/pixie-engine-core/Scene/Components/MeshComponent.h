#pragma once
#include "Component.h"
#include "Rendering/MeshHandle.h"

namespace PixieEngine {

class MeshComponent : public Component {
public:
	MeshComponent(SceneObject* parent, MeshHandle mesh);

	MeshHandle GetMesh() const;
	void SetMesh(MeshHandle mesh);

protected:
	MeshHandle m_mesh;
};

}