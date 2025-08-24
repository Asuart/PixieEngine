#pragma once
#include "pch.h"
#include "glad/glad.h"

namespace PixieEngine {

struct MeshOpenGL {
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;
	GLuint indicesCount = 0;
};

}