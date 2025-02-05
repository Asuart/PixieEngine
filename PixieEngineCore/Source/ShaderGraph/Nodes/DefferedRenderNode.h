#pragma once
#include "ShaderNode.h"
#include "ResourceManager.h"

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

	void DrawObject(SceneObject* object, Mat4 parentTransform);
	void SetupMaterial(Material* material);
};
