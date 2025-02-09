#pragma once
#include "pch.h"
#include "Resources/ResourceManager.h"

class GlobalRenderer {
public:
	static void Initialize();

	// Draw scene
	static void DrawMesh(Mesh* mesh);
	static void DrawMeshWireframe(Mesh* mesh);
	static void DrawSkybox(const Camera& camera, GLuint skyboxTexture);

	// Draw texture fitting it to screen
	static void DrawTextureFitted(GLuint id, glm::ivec2 textureResolution, glm::ivec2 viewportResolution);
	static void DrawAccumulatorTextureFitted(GLuint id, int32_t samples, glm::ivec2 textureResolution, glm::ivec2 viewportResolution);

	// Draws texture at size and coords set in screen space
	static void DrawTexture(GLuint id, Vec2 pos, Vec2 size);
	static void DrawAccumulatorTexture(GLuint id, int32_t samples, Vec2 pos, Vec2 size);

	// UI rendering
	static void DrawText(const std::string& text, Vec2 position, Float fontSize, Vec3 color = Vec3(1.0f), Mat4 projection = Mat4(1.0f), bool rightToLeft = false);
	static void DrawUIBox(Vec2 position, Vec2 size, Vec4 baseColor, Float borderRadius = 5.0f, Float borderWidth = 1.0f, Vec4 borderColor = Vec4(1.0f), Mat4 projection = Mat4(1.0f));

	// Resource Generation
	static void DrawCubeMap(const Texture& equirectangularTexture, glm::ivec2 cubemapResolution, const Cubemap& cubemapTexture, glm::ivec2 lighmapResolution, const Cubemap& lightmapTextrue, glm::ivec2 prefilterResolution, const Cubemap& prefilterMap);
	static void DrawBRDFLookUpTexture(glm::ivec2 resolution, GLuint texture);

protected:
	static Shader m_quadShader;
	static Shader m_textShader;
	static Shader m_uiBoxShader;
	static Shader m_equirectangularToCubemapShader;
	static Shader m_cubemapConvolutionShader;
	static Shader m_prefilterShader;
	static Shader m_brdfLUTShader;
	static Shader m_skyboxShader;
	static GLuint m_textVAO;
	static GLuint m_textVBO;
};