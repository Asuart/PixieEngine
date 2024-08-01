#include "PixieEngineApp.h"

PixieEngineApp::PixieEngineApp() {
	if (!gladLoadGL()) {
		std::cout << "GLEW initialization failed.\n";
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);

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

	SceneLoader sceneLoader;
	m_rtScene = sceneLoader.LoadScene("../Scenes/default.obj");

	m_rayTracingRenderer = new RayTracingRenderer(glm::ivec2(1280, 720), m_rtScene);
	m_rayTracingRenderer->StartRender();

	m_viewportFrameBuffer = new FrameBuffer();
}

PixieEngineApp::~PixieEngineApp() {
	delete m_rtScene;
	delete m_rayTracingRenderer;
	delete m_viewportFrameBuffer;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void PixieEngineApp::Start() {
	while (!m_window.IsShouldClose()) {
		glfwPollEvents();

		UserInput::Reset();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_viewportFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_rayTracingRenderer->DrawFrame();
		m_viewportFrameBuffer->Unbind();

		DrawUI();

		glfwSwapBuffers(m_window.GetGLFWWindow());
	}

	m_rayTracingRenderer->StopRender();
}

GLFWwindow* PixieEngineApp::GetGLFWWindow() {
	return m_window.GetGLFWWindow();
}

void PixieEngineApp::HandleResize(uint32_t width, uint32_t height) {
	glViewport(0, 0, width, height);
}

void PixieEngineApp::DrawUI() {
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

	if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockingOpen, windowFlags);

	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Close")) glfwSetWindowShouldClose(m_window.GetGLFWWindow(), true);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	//ImGui::ShowDemoWindow();

	m_rayTracingRenderer->DrawUI();

	ImGui::Begin("Viewport");
	ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
	if (glmViewportResolution != m_viewportResolution) {
		UpdateViewportResolution(glmViewportResolution);
	}
	ImGui::End();

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

void PixieEngineApp::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	m_rayTracingRenderer->SetViewportSize(resolution);
	m_viewportFrameBuffer->Resize(resolution.x, resolution.y);
}
