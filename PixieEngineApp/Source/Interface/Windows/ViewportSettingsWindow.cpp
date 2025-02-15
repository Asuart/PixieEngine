#include "pch.h"
#include "ViewportSettingsWindow.h"
#include "ViewportWindow.h"
#include "PixieEngineApp.h"
#include "Interface.h"

ViewportSettingsWindow::ViewportSettingsWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter) {}

void ViewportSettingsWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Viewport Settings", 0)) {
		std::vector<ViewportWindow*> viewports = m_interface.GetWindowsOfType<ViewportWindow>();
		if (viewports.size() > 0) {
			ViewportWindow* viewport = viewports[0];
			ComponentRenderer::DrawTransform(viewport->m_viewportCamera.GetTransform());
			ImGui::Spacing();

			float fovy = viewport->m_viewportCamera.GetFieldOfViewY();
			if (ImGui::DragFloat("Field of View y", &fovy, 0.01f, 0.0f, 720.0f)) {
				viewport->m_viewportCamera.SetFieldOfViewY(fovy);
			}
			ImGui::Spacing();

			if (ImGui::Checkbox("Use Camera Resolution", &viewport->m_useCameraResolution)) {}
			if (viewport->m_useCameraResolution) {
				glm::ivec2 resolution = viewport->m_viewportCamera.GetResolution();
				if (ImGui::InputInt2("Camera Resolution", &resolution[0])) {
					resolution = glm::clamp(resolution, 1, 4096);
					viewport->m_viewportCamera.SetResolution(resolution);
				}
				ImGui::Text((std::string("Aspect: ") + std::to_string(Aspect(resolution))).c_str());
			}
			ImGui::Spacing();

			RenderMode activeRenderMode = viewport->GetRenderMode();
			if (ImGui::BeginCombo("Render Mode", to_string(activeRenderMode).c_str())) {
				for (int32_t n = 0; n < (int32_t)RenderMode::COUNT; n++) {
					RenderMode mode = RenderMode(n);
					bool isSelected = (activeRenderMode == mode);
					if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
						viewport->SetRenderMode(mode);
					}
					if (isSelected) {
						activeRenderMode = mode;
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Spacing();

			if (activeRenderMode == RenderMode::PathTracing) {					
				int32_t maxRenderThreads = viewport->m_pathTracingRenderer.GetMaxRenderThreads();
				if (ImGui::InputInt("Max Render Threads", &maxRenderThreads)) {
					viewport->m_pathTracingRenderer.SetMaxRenderThreads(Clamp(maxRenderThreads, 1, 128));
				}
				ImGui::Spacing();
				
				std::string samplesText = std::string("Samples: ") + std::to_string(viewport->m_pathTracingRenderer.GetSamplesCount());
				ImGui::Text(samplesText.c_str());
				
				std::string renderTimeText = std::string("Render Time: ") + std::to_string(viewport->m_pathTracingRenderer.GetRenderTime());
				ImGui::Text(renderTimeText.c_str());
				
				std::string lastSampleTimeText = std::string("Last Sample Time: ") + std::to_string(viewport->m_pathTracingRenderer.GetLastSampleTime());
				ImGui::Text(lastSampleTimeText.c_str());
			}

			if (activeRenderMode == RenderMode::Forward || activeRenderMode == RenderMode::Deffered) {
				ViewportMode activeViewportMode = viewport->GetViewportMode();
				if (ImGui::BeginCombo("Viewport Mode", to_string(activeViewportMode).c_str())) {
					for (int32_t n = 0; n < (int32_t)ViewportMode::COUNT; n++) {
						ViewportMode mode = ViewportMode(n);
						bool isSelected = (activeViewportMode == mode);
						if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
							viewport->SetViewportMode(mode);
						}
						if (isSelected) {
							activeViewportMode = mode;
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::Spacing();

				if (activeViewportMode == ViewportMode::Stereoscopic) {
					Float distance = viewport->GetStereoscopicDistance();
					if (ImGui::DragFloat("Distance##stereoscopi", &distance, 0.001f, -10.0f, 10.0f)) {
						viewport->SetStereoscopicDistance(distance);
					}
					Vec3 balance = viewport->GetStereoscopicBalance();
					if (ImGui::DragFloat3("Balance##stereoscopi", &balance[0], 0.001f, 0.0f, 1.0f)) {
						viewport->SetStereoscopicBalance(balance);
					}
					Vec3 scale = viewport->GetStereoscopicScale();
					if (ImGui::DragFloat3("Scale##stereoscopi", &scale[0], 0.001f, 0.0f, 4.0f)) {
						viewport->SetStereoscopicScale(scale);
					}
				}
				else if (activeViewportMode == ViewportMode::VR) {
					Float distance = viewport->GetVRDistance();
					if (ImGui::DragFloat("Distance##vr", &distance, 0.001f, -10.0f, 10.0f)) {
						viewport->SetVRDistance(distance);
					}
					Float distortion = viewport->GetVRDistortion();
					if (ImGui::DragFloat("Distortion##vr", &distortion, 0.001f, -10.0f, 10.0f)) {
						viewport->SetVRDistortion(distortion);
					}
				}
			}

			if (activeRenderMode == RenderMode::Forward) {
				AntiAliasing activeAntiAlising = viewport->GetAntiAliasing();
				if (ImGui::BeginCombo("Anti Aliasing", to_string(activeAntiAlising).c_str())) {
					for (int32_t n = 0; n < (int32_t)AntiAliasing::COUNT; n++) {
						AntiAliasing mode = AntiAliasing(n);
						bool isSelected = (activeAntiAlising == mode);
						if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
							viewport->SetAntiAliasing(mode);
						}
						if (isSelected) {
							activeAntiAlising = mode;
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				ImGui::Spacing();
			}
			if (ImGui::Button("Update Scene Snapshot")) {
				SceneManager::UpdateSceneSnapshot();
			}
		}
	}
	ImGui::End();
}
