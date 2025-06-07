#pragma once
#include "ShaderGraph/ShaderNode.h"
#include "Resources/ShaderManager.h"

class DefferedRenderNode : public ShaderNode {
public:
	DefferedRenderNode();

	void Process(const Scene& scene, const Camera& camera) override;

protected:
	Shader m_program;
	GLuint m_frameBuffer = 0;
	GLuint m_depth = 0;
	Texture m_albedo;
	Texture m_normal;
	Texture m_position;
	Texture m_specular;
	Texture m_metallic;
	Texture m_roughness;

	void DrawObject(SceneObject* object, glm::mat4 parentTransform);
	void SetupMaterial(std::shared_ptr<Material> material);
};
