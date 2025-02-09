#pragma once
#include "pch.h"

class ShaderBase {
protected:
	GLuint m_programID = 0;

	ShaderBase(GLuint programID) :
		m_programID(programID) {};

public:
	inline GLuint GetProgramID() const { return m_programID; }
	inline void SetProgramID(GLuint programID) { m_programID = programID; }

	inline void Bind() const {
		glUseProgram(m_programID);
	}

	inline void Unbind() const {
		glUseProgram(0);
	}

	inline void SetUniform1i(const std::string& name, int32_t value) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform1i(loc, value);
	}

	inline void SetUniform2i(const std::string& name, glm::ivec2 v) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform2i(loc, v.x, v.y);
	}

	inline void SetUniform1iv(const std::string& name, GLint* start, int32_t count) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform1iv(loc, count, start);
	}

	inline void SetUniform1f(const std::string& name, Float value) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform1f(loc, value);
	}

	inline void SetUniform1fv(const std::string& name, GLfloat* start, int32_t count) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform1fv(loc, count, start);
	}

	inline void SetUniform2f(const std::string& name, Vec2 v) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform2f(loc, v.x, v.y);
	}

	inline void SetUniform2fv(const std::string& name, GLfloat* start, int32_t count) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform2fv(loc, count, start);
	}

	inline void SetUniform3f(const std::string& name, Vec3 v) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform3f(loc, v.x, v.y, v.z);
	}

	inline void SetUniform3fv(const std::string& name, GLfloat* start, int32_t count) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform3fv(loc, count, start);
	}

	inline void SetUniform4f(const std::string& name, Vec4 v) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform4f(loc, v.x, v.y, v.z, v.w);
	}

	inline void SetUniform4fv(const std::string& name, GLfloat* start, int32_t count) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniform4fv(loc, count, start);
	}

	inline void SetUniformMat3f(const std::string& name, const Mat3& m, GLboolean transpose = GL_FALSE) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniformMatrix3fv(loc, 1, transpose, &m[0][0]);
	}

	inline void SetUniformMat3fv(const std::string& name, const Float* start, int32_t count, GLboolean transpose = GL_FALSE) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniformMatrix3fv(loc, count, transpose, start);
	}

	inline void SetUniformMat4f(const std::string& name, const Mat4& m, GLboolean transpose = GL_FALSE) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniformMatrix4fv(loc, 1, transpose, &m[0][0]);
	}

	inline void SetUniformMat4fv(const std::string& name, const Float* start, int32_t count, GLboolean transpose = GL_FALSE) const {
		GLuint loc = glGetUniformLocation(m_programID, name.c_str());
		glUniformMatrix4fv(loc, count, transpose, start);
	}

	inline void SetTexture(const std::string& name, GLuint id, GLuint index) {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, id);
		SetUniform1i(name, index);
	}

	inline void SetCubeMap(const std::string& name, GLuint id, GLuint index) {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		SetUniform1i(name, index);
	}

	inline void SetMaterial(Material* material, int32_t startTextureIndex = 0) {
		SetTexture("albedoMap", material->m_albedoTexture.GetID(), startTextureIndex + 0);
		SetTexture("normalMap", material->m_normalTexture.GetID(), startTextureIndex + 1);
		SetTexture("metallicMap", material->m_metallicTexture.GetID(), startTextureIndex + 2);
		SetTexture("roughnessMap", material->m_roughnessTexture.GetID(), startTextureIndex + 3);
		SetTexture("aoMap", material->m_aoTexture.GetID(), startTextureIndex + 4);
		SetUniform3f("uAlbedo", material->m_albedo.GetRGB());
		SetUniform1f("uMetallic", material->m_metallic);
		SetUniform1f("uRoughness", material->m_roughness);
	}
};

class Shader : public ShaderBase {
public:
	explicit Shader(GLuint programID = 0) : 
		ShaderBase(programID) {}
};

class ComputeShader : public ShaderBase {
public:
	explicit ComputeShader(GLuint programID = 0) :
		ShaderBase(programID) {}
};
