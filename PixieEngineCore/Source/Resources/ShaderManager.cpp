#include "pch.h"
#include "ShaderManager.h"

Shader ShaderManager::LoadShader(const std::string& name) {
	return LoadShader(name + std::string("VertexShader.glsl"), name + std::string("FragmentShader.glsl"));
}

Shader ShaderManager::LoadShader(const std::string& vertexName, const std::string& fragmentName) {
	const std::string shadersPath = Globals::GetApplicationDirectory().string() + std::string("/Resources/Shaders/");
	std::string vertShaderStr = FileReader::ReadFileAsString(shadersPath + vertexName);
	std::string fragShaderStr = FileReader::ReadFileAsString(shadersPath + fragmentName);
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();
	Log::Message("Compiling shaders: %s and %s", vertexName.c_str(), fragmentName.c_str());
	return CompileShader(vertShaderSrc, fragShaderSrc);
}

Shader ShaderManager::CompileShader(const char* vertShaderSrc, const char* fragShaderSrc) {
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int32_t logLength;

	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		std::vector<char> shaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(vertShader, logLength, NULL, &shaderError[0]);
		Log::Error(&shaderError[0]);
	}

	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		std::vector<char> shaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(fragShader, logLength, NULL, &shaderError[0]);
		Log::Error(&shaderError[0]);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		std::vector<char> shaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(program, logLength, NULL, &shaderError[0]);
		Log::Error(&shaderError[0]);
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return Shader(program);
}

ComputeShader ShaderManager::LoadComputeShader(const std::string& computePath) {
	const std::string shadersPath = Globals::GetApplicationDirectory().string() + std::string("/Resources/Shaders/");
	std::string computeShaderStr = FileReader::ReadFileAsString(shadersPath + computePath);
	return CompileComputeShader(computeShaderStr.c_str());
}

ComputeShader ShaderManager::CompileComputeShader(const char* computeShaderSrc) {
	GLint result = GL_FALSE;
	int32_t logLength;
	GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(ray_shader, 1, &computeShaderSrc, NULL);
	glCompileShader(ray_shader);

	glGetShaderiv(ray_shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(ray_shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		std::vector<char> shaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(ray_shader, logLength, NULL, &shaderError[0]);
		Log::Error(&shaderError[0]);
	}

	GLuint rayProgram = glCreateProgram();
	glAttachShader(rayProgram, ray_shader);
	glLinkProgram(rayProgram);

	glGetProgramiv(rayProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(rayProgram, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		std::vector<char> shaderError((logLength > 1) ? logLength : 1);
		glGetShaderInfoLog(ray_shader, logLength, NULL, &shaderError[0]);
		Log::Error(&shaderError[0]);
	}

	return ComputeShader(rayProgram);
}
