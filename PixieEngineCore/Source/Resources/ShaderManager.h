#pragma once
#include "pch.h"
#include "Shader.h"
#include "Utils/FileReader.h"

class ShaderManager {
public:
	static Shader LoadShader(const std::string& name);
	static Shader LoadShader(const std::string& vertexName, const std::string& fragmentName);
	static Shader CompileShader(const char* vertexSource, const char* fragmentSource);
	static ComputeShader LoadComputeShader(const std::string& computePath);
	static ComputeShader CompileComputeShader(const char* computeSource);
};
