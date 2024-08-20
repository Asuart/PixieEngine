#include "pch.h"
#include "ShaderLibrary.h"

std::string ReadFile(const char* filePath) {
    std::ifstream t(filePath);
    std::string file((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return file;
}

GLuint LoadShader(const char* vertex_path, const char* fragment_path) {
    std::string vertShaderStr = ReadFile(vertex_path);
    std::string fragShaderStr = ReadFile(fragment_path);
    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();
    return CompileShader(vertShaderSrc, fragShaderSrc);
}

GLuint CompileShader(const char* vertShaderSrc, const char* fragShaderSrc) {
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int32_t logLength;

    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
    if (logLength) {
        std::cout << &vertShaderError[0] << std::endl;
    }

    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
    if (logLength) {
        std::cout << &fragShaderError[0] << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError((logLength > 1) ? logLength : 1);
    glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
    if (logLength) {
        std::cout << &programError[0] << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

GLuint LoadComputeShader(const char* compute_path) {
    GLint result = GL_FALSE;
    int logLength;

    std::string computeShaderStr = ReadFile(compute_path);
    const char* computeShaderSrc = computeShaderStr.c_str();

    GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(ray_shader, 1, &computeShaderSrc, NULL);
    glCompileShader(ray_shader);

    glGetShaderiv(ray_shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(ray_shader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(ray_shader, logLength, NULL, &fragShaderError[0]);
    std::cout << &fragShaderError[0] << std::endl;

    GLuint rayProgram = glCreateProgram();
    glAttachShader(rayProgram, ray_shader);
    glLinkProgram(rayProgram);

    glGetProgramiv(rayProgram, GL_LINK_STATUS, &result);
    glGetProgramiv(rayProgram, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError((logLength > 1) ? logLength : 1);
    glGetProgramInfoLog(rayProgram, logLength, NULL, &programError[0]);
    std::cout << &programError[0] << std::endl;

    return rayProgram;
}