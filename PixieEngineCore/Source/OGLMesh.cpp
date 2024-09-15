#include "pch.h"
#include "OGLMesh.h"

OGLMesh::OGLMesh(const Mesh& mesh) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int32_t) * mesh.indices.size(), &mesh.indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (GLvoid*)12);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT_TYPE, GL_FALSE, sizeof(Vertex), (GLvoid*)24);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	indicesCount = (uint32_t)mesh.indices.size();
}

void OGLMesh::Draw() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, NULL);
}