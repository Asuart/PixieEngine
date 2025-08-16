#pragma once
#include "pch.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "AntiAliasigUtils.h"

namespace PixieEngine {

class Renderer {
public:
	virtual void DrawFrame(std::shared_ptr<Scene> scene, const Camera& camera) const = 0;
	virtual void SetResolution(glm::ivec2 resolution) = 0;
	virtual glm::ivec2 GetResolution() const = 0;
	virtual GLuint GetFrameHandle() const = 0;
	virtual AntiAliasing GetAntiAliasing() const = 0;
	virtual void SetAntiAliasing(AntiAliasing mode) = 0;
};

}