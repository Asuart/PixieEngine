#include "pch.h"
#include "Rendering/RenderEngine.h"
#include "Physics/PhysicsEngine.h"
#include "Log.h"

class PixieEngine {
public:
	PixieEngine() = delete;

	static bool Initialize(const std::string& rootPath);
	static void Free();
};
