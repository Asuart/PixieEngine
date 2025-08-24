#pragma once
#include "pch.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "AntiAliasingEnums.h"
#include "TextureHandle.h"

namespace PixieEngine {

class RenderPipeline {
public:
	virtual void DrawFrame(std::shared_ptr<Scene> scene, const Camera& camera) const = 0;
	virtual void SetResolution(glm::ivec2 resolution) = 0;
	virtual glm::ivec2 GetResolution() const = 0;
	virtual TextureHandle GetFrameHandle() const = 0;
	virtual AntiAliasing GetAntiAliasing() const = 0;
	virtual void SetAntiAliasing(AntiAliasing mode) = 0;
};

}