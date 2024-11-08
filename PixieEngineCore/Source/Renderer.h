#pragma once
#include "pch.h"
#include "Scene.h"

class Renderer {
public:
	virtual void Reset();
	virtual void DrawFrame(Scene* scene, Camera* camera) = 0;
};
