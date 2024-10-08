#include "pch.h"
#include "PixieEngineInterface.h"
#include "PixieEngineApp.h"

PixieEngineInterface::PixieEngineInterface(PixieEngineApp& app)
	: m_app(app) {}

PixieEngineInterface::~PixieEngineInterface() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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
	ImGui::Begin("DockSpace Demo", &dockingOpen, windowFlags);

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

void PixieEngineInterface::DrawSettingsWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Settings", 0);

	ImGui::PushItemWidth(-FLT_MIN);

	if (ImGui::Checkbox("RayTracing viewport", &m_app.m_rayTracingViewport)) {
		if (m_app.m_rayTracingViewport) {
			m_app.m_rayTracingRenderer->Reset();
			m_app.m_rayTracingRenderer->StartRender();
		}
		else {
			m_app.m_rayTracingRenderer->StopRender();
		}
	}

	ImGui::Text("Scene Path");
	ImGui::InputText("##scene_path", m_app.m_scenePath, m_app.m_maxScenePathLength);
	if (ImGui::Button("Reload Scene")) {
		m_app.ReloadScene();
	}
	ImGui::Spacing();

	m_app.m_rayTracingRenderer->DrawUI();

	ImGui::PopItemWidth();

	ImGui::End();
}

void PixieEngineInterface::DrawViewportWindow() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
	ImGui::SetNextWindowSize(viewportResolution);
	ImGui::Image((void*)(uint64_t)m_app.m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
	if (glmViewportResolution != m_app.m_viewportResolution) {
		m_app.UpdateViewportResolution(glmViewportResolution);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void PixieEngineInterface::DrawMaterialsWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Materials", 0);

	std::vector<Material*> materials = m_app.m_scene->GetMaterialsList();
	for (Material* material : materials) {
		if (ImGui::CollapsingHeader(material->m_name.c_str())) {
			const Float minZero = (Float)0;
			const Float maxOne = (Float)1;
			const Float maxTen = (Float)10;

			glm::fvec3 albedoEditBuffer = material->m_albedo.GetRGB();
			if (ImGui::ColorEdit3("Albedo", &albedoEditBuffer[0])) {
				material->m_albedo.SetRGB((Vec3)albedoEditBuffer);
				m_app.m_rayTracingRenderer->Reset();
			}
			glm::fvec3 emissionColorEditBuffer = material->m_emissionColor.GetRGB();
			if (ImGui::ColorEdit3("Emission Color", &emissionColorEditBuffer[0])) {
				material->m_emissionColor.SetRGB((Vec3)emissionColorEditBuffer);
				m_app.m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragScalar("Emission Strength", ImGuiFloat, &material->m_emissionStrength)) {
				m_app.m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragScalar("Roughness", ImGuiFloat, &material->m_roughness, 0.01f, &minZero, &maxOne)) {
				m_app.m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragScalar("Metallic", ImGuiFloat, &material->m_metallic, 0.01f, &minZero, &maxOne)) {
				m_app.m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragScalar("Transparency", ImGuiFloat, &material->m_transparency, 0.01f, &minZero, &maxOne)) {
				m_app.m_rayTracingRenderer->Reset();
			}
			if (ImGui::DragScalar("Refraction", ImGuiFloat, &material->m_refraction, 0.01f, &minZero, &maxTen)) {
				m_app.m_rayTracingRenderer->Reset();
			}
		}
	}

	ImGui::End();
}

void PixieEngineInterface::DrawSceneWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Scene", 0);
	DrawSceneTree(m_app.m_scene->GetRootObject());
	ImGui::End();
}

void PixieEngineInterface::DrawInspectorWindow() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Inspector", 0);

	if (Camera* mainCamera = m_app.m_scene->GetMainCamera()) {
		ImGui::Text("Main Camera");
		ImGui::Spacing();
		DrawTransform(mainCamera->GetTransform());
		ImGui::Spacing();
	}

	if (m_app.m_selectedObject) {
		ImGui::Text(m_app.m_selectedObject->name.c_str());
		ImGui::Spacing();

		DrawTransform(m_app.m_selectedObject->transform);
		ImGui::Spacing();

		ImGui::Text("Components");
		if (m_app.m_selectedObject->components.size() > 0) {
			for (size_t i = 0; i < m_app.m_selectedObject->components.size(); i++) {
				ImGui::Text(m_app.m_selectedObject->components[i]->name.c_str());
			}
		}
		ImGui::Spacing();
	}

	ImGui::End();
}

void PixieEngineInterface::DrawSceneTree(SceneObject* object) {
	ImGuiTreeNodeFlags flags = 0;
	if (object == m_app.m_scene->GetRootObject()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
	if (object->children.size() == 0) flags |= ImGuiTreeNodeFlags_Leaf;
	if (object == m_app.m_selectedObject) flags |= ImGuiTreeNodeFlags_Selected;
	if (ImGui::TreeNodeEx(object->name.c_str(), flags)) {
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			m_app.m_selectedObject = object;
		}
		for (size_t i = 0; i < object->children.size(); i++) {
			DrawSceneTree(object->children[i]);
		}
		ImGui::TreePop();
	}
}

void PixieEngineInterface::DrawTransform(Transform& transform) {
	ImGui::Text("Transform");
	const Float minPosition = (Float)-10000, maxPosition = (Float)10000;
	const Float minRotation = (Float)-720, maxRotation = (Float)720;
	const Float minScale = (Float)-10000, maxScale = (Float)10000;
	if (ImGui::DragScalarN(("Position##" + transform.id.ToString()).c_str(), ImGuiFloat, &transform.GetPosition(), 3, 0.01f, &minPosition, &maxPosition)) {
		transform.UpdateMatrices();
	}
	if (ImGui::DragScalarN(("Rotation##" + transform.id.ToString()).c_str(), ImGuiFloat, &transform.GetRotation(), 3, 0.1f, &minRotation, &maxRotation)) {
		transform.UpdateMatrices();
	}
	if (ImGui::DragScalarN(("Scale##" + transform.id.ToString()).c_str(), ImGuiFloat, &transform.GetScale(), 3, 0.01f, &minScale, &maxScale)) {
		transform.UpdateMatrices();
	}
}