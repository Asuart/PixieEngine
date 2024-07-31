#pragma once
#include "headers.h"
#include "shaders.h"

std::string ReadFile(const char* filePath);
GLuint LoadShader(const char* vertex_path, const char* fragment_path);
GLuint CompileShader(const char* vertex_source, const char* fragment_source);
GLuint LoadComputeShader(const char* compute_path);
