#include "pch.h"
#include "PixieEngine.h"

bool PixieEngine::Initialize(const std::string& rootPath) {
	Globals::SetApplicationPath(rootPath);
	if (!RenderEngine::Initialize()) {
		Log::Error("Failed to initialize Render Engine");
		return false;
	}
	if (!PhysicsEngine::Initialize()) {
		Log::Error("Failed to initialize Physics Engine");
		return false;
	}
	return true;
}

void PixieEngine::Free() {
	RenderEngine::Free();
	PhysicsEngine::Free();
}
