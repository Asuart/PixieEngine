#ifndef PIXIE_ENGINE_OPENGL_MESH
#define PIXIE_ENGINE_OPENGL_MESH

#include "pch.h"
#include "Mesh.h"

class OGLMesh {
public:
	GLuint shader = 0;

	OGLMesh(const Mesh& mesh);

	void Draw();

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;
	uint32_t indicesCount = 0;
};

#endif // PIXIE_ENGINE_OPENGL_MESH