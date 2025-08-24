#pragma once
#include "pch.h"
#include "RenderAPI.h"
#include "Scene/Camera.h"
#include "Resources/FontLoader.h"
#include "MeshHandle.h"
#include "RendererBase.h"
#include "TextureHandle.h"
#include "RenderPipeline.h"
#include "Utils/Buffer2D.h"

namespace PixieEngine {

class RenderEngine {
public:
	static bool Initialize(RenderAPI api);
	static void Free();

	static RenderAPI GetRenderAPI();
	static RendererBase* GetRenderer();
	static RenderPipeline* GetPipeline();

	static void StartFrame();
	static void EndFrame();

	static MeshHandle LoadMesh(const Mesh& mesh);
	static TextureHandle LoadTexture(const Buffer2D<float>& texture);
	static TextureHandle LoadTexture(const Buffer2D<glm::vec3>& texture);
	static TextureHandle LoadTexture(const Buffer2D<glm::vec4>& texture);

	// Draw scene
	//static void DrawMesh(MeshHandle mesh);
	//static void DrawSkybox(const Camera& camera, const Skybox& skybox);
	//static void DrawQuad();
	//static void DrawCube();
	//static void DrawSphere();

	// Draw texture as quad
	//static void DrawTexture(TextureHandle texture, glm::vec2 pos, glm::vec2 size);
	//static void DrawTextureFitted(TextureHandle texture, glm::ivec2 textureResolution, glm::ivec2 viewportResolution);

	// UI rendering
	//static void DrawText(const std::string& text, glm::vec2 position, float fontSize, glm::vec3 color = glm::vec3(1.0f), glm::mat4 projection = glm::mat4(1.0f), bool rightToLeft = false);

	// Resource Generation
	//static void DrawCubeMap(const Texture& equirectangularTexture, glm::ivec2 cubemapResolution, const Cubemap& cubemapTexture, glm::ivec2 lighmapResolution, const Cubemap& lightmapTextrue, glm::ivec2 prefilterResolution, const Cubemap& prefilterMap);

protected:
	inline static RenderAPI s_renderAPI = RenderAPI::Undefined;
	inline static RendererBase* s_renderer = nullptr;
	inline static RenderPipeline* s_pipeline = nullptr;
};

}