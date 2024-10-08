#include "pch.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh* mesh, SceneObject* parent)
	: Component("Mesh Component", parent), mesh(mesh) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->vertices.size(), mesh->vertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLFloatType)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLFloatType)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);               
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

MeshComponent::~MeshComponent() {

}

const Mesh* MeshComponent::GetMesh() const {
	return mesh;
}

void MeshComponent::Draw() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
