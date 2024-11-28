#include "pch.h"
#include "PixieEngineApp.h"
#include "OpenGLInterface.h"

PixieEngineApp::PixieEngineApp() :
	m_interface(*this) {
	if (!gladLoadGL()) {
		std::cout << "GLEW initialization failed.\n";
		exit(1);
	}
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(openglCallbackFunction, 0);

	glEnable(GL_DEPTH_TEST);

	ResourceManager::Initialize();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window.GetGLFWWindow(), true);
	ImGui_ImplOpenGL3_Init();

	m_scene = ResourceManager::LoadScene(m_currentScenePath);
	if (!m_scene) {
		m_scene = new Scene("Scene");
		SceneObject* object = ResourceManager::LoadModel(m_currentScenePath);
		if (object) {
			m_scene->AddObject(object);
		}
	}
	m_scene->Start();
	m_sceneSnapsot = new SceneSnapshot(m_scene);

	m_defaultRenderer = new DefaultRenderer();

	m_interface.Initialize();
}

PixieEngineApp::~PixieEngineApp() {
	delete m_defaultRenderer;
	if (m_sceneSnapsot) {
		delete m_sceneSnapsot;
	}
	if (m_scene) {
		delete m_scene;
	}
}

void PixieEngineApp::Start() {
	while (!m_window.IsShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Time::Update();
		UserInput::Reset();
		glfwPollEvents();
		HandleUserInput();
		m_scene->Update();
		m_interface.Draw();
		glfwSwapBuffers(m_window.GetGLFWWindow());
	}
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		rayTracingWindows[i]->StopRender();
	}
}

GLFWwindow* PixieEngineApp::GetGLFWWindow() {
	return m_window.GetGLFWWindow();
}

void PixieEngineApp::HandleResize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void PixieEngineApp::ResetRayTracers() {
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		rayTracingWindows[i]->Reset();
	}
}

Scene* PixieEngineApp::GetScene() {
	return m_scene;
}

SceneSnapshot* PixieEngineApp::GetSceneSnapshot() {
	return m_sceneSnapsot;
}

const SceneObject* PixieEngineApp::GetSelectedObject() const {
	return m_selectedObject;
}

SceneObject* PixieEngineApp::GetSelectedObject() {
	return m_selectedObject;
}

void PixieEngineApp::SelectObject(SceneObject* object) {
	m_selectedObject = object;
}

void PixieEngineApp::RemoveSelectedObject() {
	if (!m_selectedObject || m_selectedObject == m_scene->GetRootObject()) {
		return;
	}
	m_selectedObject->Detach();
	delete m_selectedObject;
	m_selectedObject = nullptr;
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	std::vector<bool> renderingStates;
	renderingStates.resize(rayTracingWindows.size());
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		renderingStates[i] = rayTracingWindows[i]->IsRendering();
		rayTracingWindows[i]->StopRender();
		rayTracingWindows[i]->Reset();
	}
	UpdateSceneSnapshot();
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		if (renderingStates[i]) {
			rayTracingWindows[i]->StartRender();
		}
	}
}

void PixieEngineApp::RestoreViewportSize() {
	glViewport(0, 0, m_window.GetWindowSize().x, m_window.GetWindowSize().y);
}

DefaultRenderer* PixieEngineApp::GetDefaultRenderer() {
	return m_defaultRenderer;
}

const std::filesystem::path& PixieEngineApp::GetAssetsPath() {
	return m_assetsPath;
}

void PixieEngineApp::SetAssetsPath(const std::filesystem::path& path) {
	m_assetsPath = path;
}

void PixieEngineApp::UpdateSceneSnapshot() {
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	std::vector<bool> renderingStates;
	renderingStates.resize(rayTracingWindows.size());
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		renderingStates[i] = rayTracingWindows[i]->IsRendering();
		rayTracingWindows[i]->StopRender();
		rayTracingWindows[i]->Reset();
	}
	if (m_sceneSnapsot) {
		delete m_sceneSnapsot;
	}
	m_sceneSnapsot = new SceneSnapshot(m_scene);
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		if (renderingStates[i]) {
			rayTracingWindows[i]->StartRender();
		}
	}
}

void PixieEngineApp::LoadDemoScene(GeneratedScene type) {
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	std::vector<bool> renderingStates;
	renderingStates.resize(rayTracingWindows.size());
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		renderingStates[i] = rayTracingWindows[i]->IsRendering();
		rayTracingWindows[i]->StopRender();
		rayTracingWindows[i]->Reset();
	}
	if (m_scene) {
		delete m_scene;
	}
	m_scene = SceneGenerator::CreateScene(type);
	m_scene->Start();
	UpdateSceneSnapshot();
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		if (renderingStates[i]) {
			rayTracingWindows[i]->StartRender();
		}
	}
}

void PixieEngineApp::AddObject(GeneratedObject type) {
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	std::vector<bool> renderingStates;
	renderingStates.resize(rayTracingWindows.size());
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		renderingStates[i] = rayTracingWindows[i]->IsRendering();
		rayTracingWindows[i]->StopRender();
		rayTracingWindows[i]->Reset();
	}
	SceneObject* object = SceneGenerator::CreateObject(type);
	if (object) {
		m_scene->AddObject(object);
		object->OnStart();
		UpdateSceneSnapshot();
	}
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		if (renderingStates[i]) {
			rayTracingWindows[i]->StartRender();
		}
	}
}

void PixieEngineApp::LoadScene(const std::filesystem::path& filePath) {
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	std::vector<bool> renderingStates;
	renderingStates.resize(rayTracingWindows.size());
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		renderingStates[i] = rayTracingWindows[i]->IsRendering();
		rayTracingWindows[i]->StopRender();
		rayTracingWindows[i]->Reset();
	}
	if (m_scene) {
		delete m_scene;
	}
	m_scene = ResourceManager::LoadScene(filePath);
	if (!m_scene) {
		m_scene = new Scene("Scene");
	}
	else {
		m_currentScenePath = filePath;
	}
	m_scene->Start();
	UpdateSceneSnapshot();
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		if (renderingStates[i]) {
			rayTracingWindows[i]->StartRender();
		}
	}
}

void PixieEngineApp::LoadModel(const std::filesystem::path& filePath) {
	SceneObject* object = ResourceManager::LoadModel(filePath);
	if (object) {
		std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
		std::vector<bool> renderingStates;
		renderingStates.resize(rayTracingWindows.size());
		for (size_t i = 0; i < rayTracingWindows.size(); i++) {
			renderingStates[i] = rayTracingWindows[i]->IsRendering();
			rayTracingWindows[i]->StopRender();
			rayTracingWindows[i]->Reset();
		}
		m_scene->GetRootObject()->AddChild(object);
		object->OnStart();
		if (m_sceneSnapsot) {
			delete m_sceneSnapsot;
		}
		m_sceneSnapsot = new SceneSnapshot(m_scene);
		for (size_t i = 0; i < rayTracingWindows.size(); i++) {
			if (renderingStates[i]) {
				rayTracingWindows[i]->StartRender();
			}
		}
	}
}

void PixieEngineApp::ReloadScene() {
	std::vector<RayTracingViewportWindow*> rayTracingWindows = m_interface.GetWindowsOfType<RayTracingViewportWindow>();
	std::vector<bool> renderingStates;
	renderingStates.resize(rayTracingWindows.size());
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		renderingStates[i] = rayTracingWindows[i]->IsRendering();
		rayTracingWindows[i]->StopRender();
		rayTracingWindows[i]->Reset();
	}
	if (m_scene) {
		delete m_scene;
	}
	m_scene = ResourceManager::LoadScene(m_currentScenePath);
	m_scene->Start();
	if (m_sceneSnapsot) {
		delete m_sceneSnapsot;
	}
	m_sceneSnapsot = new SceneSnapshot(m_scene);
	for (size_t i = 0; i < rayTracingWindows.size(); i++) {
		if (renderingStates[i]) {
			rayTracingWindows[i]->StartRender();
		}
	}
}

void PixieEngineApp::HandleUserInput() {}
