#pragma once
#include "pch.h"

class SceneSnapshotVisualizer {
public:
	SceneSnapshotVisualizer(glm::ivec2 resolution);

	void Draw(Scene* scene, const SceneSnapshot& sceneSnapshot, const Camera& camera);

public:
	GLuint m_opaqueFBO;
	GLuint m_transparentFBO;
	GLuint m_opaqueTexture;
	GLuint m_depthTexture;
	GLuint m_accumTexture;
	GLuint m_revealTexture;
	Shader m_solidShader;
	Shader m_transparentShader;
	Shader m_compositeShader;

	void DrawObject(SceneObject* object, Mat4 parentTransform = Mat4(1.0f));
};