#include "pch.h"
#include "PixieEngineInterface.h"
#include "PixieEngineApp.h"

PixieEngineInterface::PixieEngineInterface(PixieEngineApp& app) :
	m_app(app) {
	m_openWindows.push_back(new SceneTreeWindow(m_app, *this));
	m_openWindows.push_back(new AssetsBrowserWindow(m_app, *this));
	m_openWindows.push_back(new InspectorWindow(m_app, *this));
	m_openWindows.push_back(new MaterialsBrowserWindow(m_app, *this));
	m_openWindows.push_back(new RayTracingSettingsWindow(m_app, *this));
	m_openWindows.push_back(new DefaultViewportWindow(m_app, *this));
	m_openWindows.push_back(new RayTracingViewportWindow(m_app, *this));
	m_openWindows.push_back(new StereoscopicViewportWindow(m_app, *this));
	m_openWindows.push_back(new StereoscopicViewportSettingsWindow(m_app, *this));
	m_openWindows.push_back(new VRViewportWindow(m_app, *this));
	m_openWindows.push_back(new VRViewportSettingsWindow(m_app, *this));
	m_openWindows.push_back(new StatsWindow(m_app, *this));
	m_openWindows.push_back(new RendererViewportSettingsWindow(m_app, *this));
	m_openWindows.push_back(new GPURayTracingViewportWindow(m_app, *this));
	m_openWindows.push_back(new DefferedViewportWindow(m_app, *this));
}

PixieEngineInterface::~PixieEngineInterface() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void PixieEngineInterface::Initialize() {
	ComponentRenderer::Initialize(&m_app);
	for (size_t i = 0; i < m_openWindows.size(); i++) {
		m_openWindows[i]->Initialize();
	}
}

void PixieEngineInterface::Draw() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool dockingOpen = true;
	static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
		windowFlags |= ImGuiWindowFlags_NoBackground;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &dockingOpen, windowFlags);

	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Close")) {
				glfwSetWindowShouldClose(m_app.m_window.GetGLFWWindow(), true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Test Scenes")) {
			if (ImGui::MenuItem("Empty Scene")) {
				m_app.LoadDemoScene(GeneratedScene::Empty);
			}
			if (ImGui::MenuItem("Demo Sphere")) {
				m_app.LoadDemoScene(GeneratedScene::DemoSphere);
			}
			if (ImGui::MenuItem("Small And Big Spheres")) {
				m_app.LoadDemoScene(GeneratedScene::SmallAndBigSpheres);
			}
			if (ImGui::MenuItem("Test Materials")) {
				m_app.LoadDemoScene(GeneratedScene::TestMaterials);
			}
			if (ImGui::MenuItem("Randomized Spheres")) {
				m_app.LoadDemoScene(GeneratedScene::RandomizedSpheres);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add Object")) {
			if (ImGui::MenuItem("Sphere")) {
				m_app.AddObject(GeneratedObject::Sphere);
			}
			if (ImGui::MenuItem("Point Light")) {
				m_app.AddObject(GeneratedObject::PointLight);
			}
			if (ImGui::MenuItem("Directional Light")) {
				m_app.AddObject(GeneratedObject::DirectionalLight);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::ShowDemoWindow();

	for (size_t i = 0; i < m_openWindows.size(); i++) {
		m_openWindows[i]->Draw();
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
