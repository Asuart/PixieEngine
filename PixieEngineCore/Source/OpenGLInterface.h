#pragma once

inline void glUniformMatrix3(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
	glUniformMatrix3fv(location, count, transpose, value);
}

inline void glUniformMatrix3(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) {
	glUniformMatrix3dv(location, count, transpose, value);
}

inline void glUniformMatrix4(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
	glUniformMatrix4fv(location, count, transpose, value);
}

inline void glUniformMatrix4(GLint location, GLsizei count, GLboolean transpose, const GLdouble* value) {
	glUniformMatrix4dv(location, count, transpose, value);
}

inline void glUniform1(GLint location, GLfloat value) {
	glUniform1f(location, value);
}

inline void glUniform1(GLint location, GLdouble value) {
	glUniform1d(location, value);
}

inline void glUniform3(GLint location, GLfloat x, GLfloat y, GLfloat z) {
	glUniform3f(location, x, y, z);
}

inline void glUniform3(GLint location, GLdouble x, GLdouble y, GLdouble z) {
	glUniform3d(location, x, y, z);
}

inline void glUniform3(GLint location, Vec3 v) {
	glUniform3(location, v.x, v.y, v.z);
}

inline void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	// filter some warnings
	if (id == 131185 || id == 131218) return;

	std::cout << "---------------------opengl-callback-start------------\n";
	std::cout << "message: " << message << "\n";
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	default:
		std::cout << "UNDEFINED";
	}
	std::cout << "\n";

	std::cout << "id: " << id << "\n";
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	default:
		std::cout << "UNDEFINED";
	}
	std::cout << "\n---------------------opengl-callback-end--------------\n";
}
