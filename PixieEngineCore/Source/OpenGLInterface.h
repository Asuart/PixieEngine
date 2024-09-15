#pragma once

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
