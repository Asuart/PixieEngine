#include "pch.h"
#include "PixieEngineApp.h"

PixieEngineApp app;

void window_size_callback(GLFWwindow* window, int32_t width, int32_t height) {
	app.HandleWindowResize(width, height);
}

int32_t main(int32_t argc, char** argv) {
	glfwSetWindowSizeCallback(app.GetGLFWWindow(), window_size_callback);
	
	if (!PixieEngine::Initialize(argv[0])) {
		Log::Error("Failed to initialize Pixie Engine");
		return 1;
	}

	app.Initialize();
	app.Start();

	PixieEngine::Free();

	return 0;
}
