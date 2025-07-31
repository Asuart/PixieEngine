#pragma once
#include "pch.h"
#include "ShaderNodeIO.h"
#include "Scene/Scene.h"

class ShaderNode {
public:
	ShaderNode(const std::string& name);

	virtual void Process(const Scene& scene, const Camera& camera) {};

	const std::string& GetName() const;
	std::vector<ShaderNodeInput>& GetInputs();
	std::vector<ShaderNodeOutput>& GetOutputs();

	ShaderNodeInput* GetInput(const std::string& name);
	int32_t GetInputInt(const std::string& name);
	float GetInputFloat(const std::string& name);
	glm::vec2 GetInputVec2(const std::string& name);
	glm::vec3 GetInputVec3(const std::string& name);
	glm::vec4 GetInputVec4(const std::string& name);
	GLuint GetInputTexture(const std::string& name);

	ShaderNodeOutput* GetOutput(const std::string& name);
	int32_t GetOutputInt(const std::string& name);
	float GetOutputFloat(const std::string& name);
	glm::vec2 GetOutputVec2(const std::string& name);
	glm::vec3 GetOutputVec3(const std::string& name);
	glm::vec4 GetOutputVec4(const std::string& name);
	GLuint GetOutputTexture(const std::string& name);

protected:
	std::string m_name;
	std::vector<ShaderNodeInput> m_inputs;
	std::vector<ShaderNodeOutput> m_outputs;
};
