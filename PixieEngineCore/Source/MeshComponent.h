#pragma once
#include "Mesh.h"
#include "Component.h"
#include "Material.h"

class MeshComponent : public Component {
public:
	MeshComponent(Mesh* mesh, const SceneObject* parent);
	~MeshComponent();

	const Mesh* GetMesh() const;
	void Draw();

protected:
	Mesh* mesh;
	GLuint vao;
	GLuint vertexBufferObject;
	GLuint indexBufferObject;
};
