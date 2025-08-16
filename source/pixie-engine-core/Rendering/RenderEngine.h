#pragma once
#include "pch.h"
#include "RenderAPI.h"
#include "FrameBuffer.h"
#include "Scene/Camera.h"
#include "Resources/Cubemap.h"
#include "Resources/ShaderManager.h"
#include "Resources/FontLoader.h"
#include "Resources/MeshHandle.h"

namespace PixieEngine {

class RenderEngine {
public:
	static bool Initialize(RenderAPI api);
	static void Free();

	static RenderAPI GetRenderAPI();

	// Draw scene
	static void DrawMesh(const MeshHandle& mesh);
	static void DrawMeshWireframe(const MeshHandle& mesh);
	static void DrawSkybox(const Camera& camera, GLuint skyboxTexture);
	static void DrawQuad();
	static void DrawCube();
	static void DrawSphere();

	// Draw texture fitting it to screen
	static void DrawTextureFitted(GLuint id, glm::ivec2 textureResolution, glm::ivec2 viewportResolution);
	static void DrawAccumulatorTextureFitted(GLuint id, int32_t samples, glm::ivec2 textureResolution, glm::ivec2 viewportResolution);

	// Draws texture at size and coords set in screen space
	static void DrawTexture(GLuint id, glm::vec2 pos, glm::vec2 size);
	static void DrawAccumulatorTexture(GLuint id, int32_t samples, glm::vec2 pos, glm::vec2 size);

	// UI rendering
	static void DrawText(const std::string& text, glm::vec2 position, float fontSize, glm::vec3 color = glm::vec3(1.0f), glm::mat4 projection = glm::mat4(1.0f), bool rightToLeft = false);
	static void DrawUIBox(glm::vec2 position, glm::vec2 size, glm::vec4 baseColor, float borderRadius = 5.0f, float borderWidth = 1.0f, glm::vec4 borderColor = glm::vec4(1.0f), glm::mat4 projection = glm::mat4(1.0f));

	// Resource Generation
	static void DrawCubeMap(const Texture& equirectangularTexture, glm::ivec2 cubemapResolution, const Cubemap& cubemapTexture, glm::ivec2 lighmapResolution, const Cubemap& lightmapTextrue, glm::ivec2 prefilterResolution, const Cubemap& prefilterMap);
	static void DrawBRDFLookUpTexture(glm::ivec2 resolution, GLuint texture);

	// Get common resources
	static GLuint GetBRDFLUTHandle();

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
	static Texture* m_brdfLUT;
	static MeshHandle* m_quadMesh;
	static MeshHandle* m_cubeMesh;
	static MeshHandle* m_sphereMesh;
	static std::map<char, FontCharacter> m_characters;
	static uint32_t m_defaultFontSize;

	inline static RenderAPI m_renderAPI = RenderAPI::Undefined;
};

}