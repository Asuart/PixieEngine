#pragma once
#include "pch.h"

namespace PixieEngine {

struct ShaderProgramOpenGL {
	GLuint id;
};

ShaderProgramOpenGL CompileShaderOpenGL(const std::string& vertexShaderSource, const std::string& framgentShaderSource);

ShaderProgramOpenGL CompileComputeShaderOpenGL(const std::string& computeShaderSource);

}