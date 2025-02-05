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
	GLuint m_albedo = 0;
	GLuint m_normal = 0;
	GLuint m_position = 0;
	GLuint m_specular = 0;
	GLuint m_metallic = 0;
	GLuint m_roughness = 0;
	GLuint m_depth = 0;

	void DrawObject(SceneObject* object, Mat4 parentTransform);
	void SetupMaterial(Material* material);
};
