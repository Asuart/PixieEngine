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
	m_scene = sceneLoader.LoadScene(m_scenePath);
	m_rtScene = RTScene::FromScene(m_scene);

	m_sceneRenderer = new SceneRenderer(glm::ivec2(1280, 720), m_scene);

	m_rayTracingRenderer = new RayTracingRenderer(this, glm::ivec2(1280, 720), m_rtScene);
	m_rayTracingRenderer->StartRender();

	m_viewportFrameBuffer = new FrameBuffer(1280, 720);
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
		glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
		m_rayTracingRenderer->DrawFrame();
		//m_sceneRenderer->DrawFrame();
		m_viewportFrameBuffer->Unbind();
		glViewport(0, 0, m_window.GetWindowSize().x, m_window.GetWindowSize().y);

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
	DrawSettingsWindow();
	DrawViewportWindow();
	DrawSceneWindow();
	DrawMaterialsWindow();
	DrawInspectorWindow();

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

void PixieEngineApp::DrawSettingsWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Settings", 0);

	ImGui::PushItemWidth(-FLT_MIN);

	ImGui::Text("Scene Path");
	ImGui::InputText("##scene_path", m_scenePath, m_maxScenePathLength);
	if (ImGui::Button("Reload Scene")) {
		ReloadScene();
	}
	ImGui::Spacing();

	m_rayTracingRenderer->DrawUI();

	ImGui::PopItemWidth();

	ImGui::End();
}

void PixieEngineApp::DrawViewportWindow() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
	ImGui::SetNextWindowSize(viewportResolution);
	ImGui::Image((void*)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
	if (glmViewportResolution != m_viewportResolution) {
		UpdateViewportResolution(glmViewportResolution);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void PixieEngineApp::DrawSceneWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Scene", 0);
	DrawSceneTree(m_scene->GetRootObject());
	ImGui::End();
}

void PixieEngineApp::DrawSceneTree(SceneObject* object) {
	ImGuiTreeNodeFlags flags = 0;
	if (object == m_scene->GetRootObject()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
	if (object->children.size() == 0) flags |= ImGuiTreeNodeFlags_Leaf;
	if (object == m_selectedObject) flags |= ImGuiTreeNodeFlags_Selected;
	if (ImGui::TreeNodeEx(object->name.c_str(), flags)) {
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			m_selectedObject = object;
		}
		for (size_t i = 0; i < object->children.size(); i++) {
			DrawSceneTree(object->children[i]);
		}
		ImGui::TreePop();
	}
}

void PixieEngineApp::DrawMaterialsWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Materials", 0);

	std::vector<Material*> materials = m_scene->GetMaterialsList();
	for (Material* material : materials) {
		if (ImGui::CollapsingHeader(material->name.c_str())) {
			if (ImGui::ColorEdit3("Albedo", (float*)&material->albedo)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::ColorEdit3("Emission", (float*)&material->emission)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Roughness", &material->roughness, 0.01f, 0.0f, 1.0f)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Metallic", &material->metallic, 0.01f, 0.0f, 1.0f)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Transparency", &material->transparency, 0.01f, 0.0f, 1.0f)) {
				m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragFloat("Refraction", &material->eta, 0.01f, 0.0f, 10.0f)) {
				m_rayTracingRenderer->Reset();
			}
		}
	}

	ImGui::End();
}

void PixieEngineApp::DrawInspectorWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Inspector", 0);
	if (!m_selectedObject) {
		ImGui::End();
		return;
	}

	ImGui::Text(m_selectedObject->name.c_str());
	ImGui::Spacing();

	Transform& transform = m_selectedObject->transform;
	ImGui::Text("Transform");
	if (ImGui::DragFloat3("Position", (float*)&transform.position, 0.01f, -10000.0, 10000.0)) {
		transform.UpdateMatrices();
	}
	if (ImGui::DragFloat3("Rotation", (float*)&transform.rotation, 0.01f, -10000.0, 10000.0)) {
		transform.UpdateMatrices();
	}
	if (ImGui::DragFloat3("Scale", (float*)&transform.scale, 0.01f, -10000.0, 10000.0)) {
		transform.UpdateMatrices();
	}
	ImGui::Spacing();

	ImGui::Text("Components");
	if (m_selectedObject->components.size() > 0) {
		for (size_t i = 0; i < m_selectedObject->components.size(); i++) {
			ImGui::Text(m_selectedObject->components[i]->name.c_str());
		}
	}
	ImGui::Spacing();

	ImGui::End();
}

void PixieEngineApp::UpdateViewportResolution(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	m_rayTracingRenderer->SetViewportSize(resolution);
	m_viewportFrameBuffer->Resize(resolution.x, resolution.y);
}

void PixieEngineApp::ReloadScene() {
	m_rayTracingRenderer->StopRender();
	if (m_rtScene) {
		delete m_rtScene;
		m_rtScene = nullptr;
	}
	if (m_scene) {
		delete m_scene;
		m_scene = nullptr;
	}

	SceneLoader sceneLoader;
	m_scene = sceneLoader.LoadScene(m_scenePath);
	m_rtScene = RTScene::FromScene(m_scene);

	m_rayTracingRenderer->SetScene(m_rtScene);
	m_rayTracingRenderer->StartRender();
}