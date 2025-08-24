#pragma once
#include "pch.h"
#include <glad/glad.h>

namespace PixieEngine {

struct ShaderProgramOpenGL {
	GLuint id;
};

struct ComputeShaderProgramOpenGL {
	GLuint id;
};

ShaderProgramOpenGL CompileShaderOpenGL(const char* vertexShaderSource, const char* framgentShaderSource, const char* geometryShaderSource);

ComputeShaderProgramOpenGL CompileComputeShaderOpenGL(const char* computeShaderSource);

}