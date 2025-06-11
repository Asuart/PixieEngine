#include "pch.h"
#include "Interface.h"
#include "PixieEngineApp.h"
#include "AssetsBrowserWindow.h"
#include "SceneTreeWindow.h"
#include "InspectorWindow.h"
#include "StatsWindow.h"
#include "ViewportSettingsWindow.h"
#include "ViewportWindow.h"
#include "ShaderGraphWindow.h"
#include "ParticleSimulation3DWindow.h"

Interface::Interface(PixieEngineApp& app) :
	m_app(app), m_menu(app, *this) {}

Interface::~Interface() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Interface::Initialize() {
	m_openWindows.push_back(new SceneTreeWindow(m_app, *this));
	m_openWindows.push_back(new AssetsBrowserWindow(m_app, *this));
	m_openWindows.push_back(new InspectorWindow(m_app, *this));
	m_openWindows.push_back(new StatsWindow(m_app, *this));
	m_openWindows.push_back(new ViewportSettingsWindow(m_app, *this));
	//m_openWindows.push_back(new ShaderGraphWindow(m_app, *this));
	m_openWindows.push_back(new ViewportWindow(m_app, *this));
	m_openWindows.push_back(new ParticleSimulation3DWindow(m_app, *this));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_app.m_mainWindow.GetGLFWWindow(), true);
	ImGui_ImplOpenGL3_Init();

	for (size_t i = 0; i < m_openWindows.size(); i++) {
		m_openWindows[i]->Initialize();
	}
}

void Interface::Draw() {
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

	m_menu.Draw();

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
