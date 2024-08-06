#include "RayTracingRenderer.h"

RayTracingRenderer::RayTracingRenderer(PixieEngineApp* parent, glm::ivec2 resolution, RTScene* scene)
	: m_parent(parent), m_resolution(resolution), m_rayTracer(new RandomWalkIntegrator({1280, 720})), m_scene(scene), m_viewportResolution(resolution) {
	m_rayTracer->SetScene(scene);
	//m_rayTracer->mode = CPURayTracerMode::LiPath;
}

RayTracingRenderer::~RayTracingRenderer() {
	delete m_rayTracer;
}

void RayTracingRenderer::DrawFrame() {
	GLuint program = m_rayTracer->m_film.mesh->shader;
	GLuint posLoc = glGetUniformLocation(program, "uPos");
	GLuint sizeLoc = glGetUniformLocation(program, "uSize");
	GLuint samplesLoc = glGetUniformLocation(program, "uSamples");

	float textureAspect = (float)800 / 600;
	float viewportAspect = (float)m_viewportResolution.x / m_viewportResolution.y;
	float posX, posY;
	float sizeX, sizeY;
	if (viewportAspect > textureAspect) {
		sizeY = 1.0f;
		sizeX = textureAspect / viewportAspect;
		posX = (1.0f - sizeX) * 0.5f;
		posY = 0.0f;
	}
	else {
		sizeX = 1.0f;
		sizeY = viewportAspect / textureAspect;
		posX = 0.0f;
		posY = (1.0f - sizeY) * 0.5f;
	}

	m_rayTracer->m_film.texture->Upload();

	glUseProgram(program);
	glUniform2f(posLoc, posX, posY);
	glUniform2f(sizeLoc, sizeX, sizeY);
	glUniform1f(samplesLoc, m_rayTracer->GetSamplesCount());
	glActiveTexture(GL_TEXTURE0);
	m_rayTracer->m_film.texture->Bind(GL_TEXTURE0);
	m_rayTracer->m_film.mesh->Draw();
}

void RayTracingRenderer::Reset() {
	m_rayTracer->Reset();
}

void RayTracingRenderer::SetScene(RTScene* scene) {
	m_scene = scene;
	m_rayTracer->SetScene(scene);
	for (Camera& camera : m_scene->cameras) {
		camera.aspect = (float)m_resolution.x / m_resolution.y;
	}
}

void RayTracingRenderer::StartRender() {
	m_rayTracer->StartRender();
}

void RayTracingRenderer::StopRender() {
	m_rayTracer->StopRender();
}

void RayTracingRenderer::DrawUI() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Demo Select", 0);

	ImGui::Text("Scene Path");
	ImGui::InputText("##scene_path", m_parent->m_scenePath, m_parent->m_maxScenePathLength);
	if (ImGui::Button("Reload Scene")) {
		m_parent->ReloadScene();
	}
	ImGui::Spacing();

	//if (ImGui::BeginCombo("Render Mode", to_string(m_rayTracer->mode).c_str())) {
	//	for (int n = 0; n < (int32_t)CPURayTracerMode::_COUNT_; n++) {
	//		CPURayTracerMode mode = CPURayTracerMode(n);
	//		bool is_selected = (m_rayTracer->mode == mode);
	//		if (ImGui::Selectable(to_string(mode).c_str(), is_selected)) {
	//			m_rayTracer->mode = mode;
	//			m_rayTracer->Reset();
	//		}
	//		if (is_selected) {
	//			ImGui::SetItemDefaultFocus();
	//		}
	//	}
	//	ImGui::EndCombo();
	//}

	ImGui::Text("Render Resolution");
	if (ImGui::InputInt2("##render_resolution", (int*)&m_resolution)) {
		m_rayTracer->SetResolution(m_resolution);
		for (Camera& camera : m_scene->cameras) {
			camera.aspect = (float)m_resolution.x / m_resolution.y;
		}
	}
	ImGui::Spacing();

	ImGui::Text("Max Ray Bounces");
	if (ImGui::InputInt("##max-ray_bounces", &m_rayTracer->m_maxDepth)) {
		m_rayTracer->m_maxDepth = Clamp(m_rayTracer->m_maxDepth, 0, 99999);
		Reset();
	}
	ImGui::Spacing();


	std::string activeCameraName = "Camera ";
	for (size_t i = 0; i < m_scene->cameras.size(); i++) {
		if (m_scene->mainCamera == &m_scene->cameras[i]) {
			activeCameraName += std::to_string(i);
			break;
		}
	}
	ImGui::Text("Active Camera");
	if (ImGui::BeginCombo("##active_camera", activeCameraName.c_str())) {
		for (size_t i = 0; i < m_scene->cameras.size(); i++) {
			bool isSelected = m_scene->mainCamera == &m_scene->cameras[i];
			if (ImGui::Selectable(("Camera " + std::to_string(i)).c_str(), isSelected)) {
				m_scene->mainCamera = &m_scene->cameras[i];
				m_rayTracer->Reset();
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();

	//if (m_rayTracer->mode == CPURayTracerMode::TraceRay) {
	//	if (ImGui::Checkbox("Sample Lights", &m_rayTracer->sampleLights)) {
	//		Reset();
	//	}
	//}
	//else if (m_rayTracer->mode == CPURayTracerMode::LiSimplePath) {
	//	if (ImGui::Checkbox("Sample Lights", &m_rayTracer->sampleLights)) {
	//		Reset();
	//	}
	//	if (ImGui::Checkbox("Sample BSDF", &m_rayTracer->sampleBSDF)) {
	//		Reset();
	//	}
	//}
	//else if (m_rayTracer->mode == CPURayTracerMode::LiPath) {
	//	if (ImGui::Checkbox("Regularize BSDF", &m_rayTracer->regularize)) {
	//		Reset();
	//	}
	//}

	std::string threadsText = std::string("Threads: ") + std::to_string(m_rayTracer->GetThreadsCount());
	ImGui::Text(threadsText.c_str());
	
	std::string samplesText = std::string("Samples: ") + std::to_string(m_rayTracer->GetSamplesCount());
	ImGui::Text(samplesText.c_str());
	
	std::string renderTimeText = std::string("Render Time: ") + std::to_string(m_rayTracer->GetRenderTime());
	ImGui::Text(renderTimeText.c_str());
	
	std::string lastSampleTimeText = std::string("Last Sample Time: ") + std::to_string(m_rayTracer->GetLastSampleTime());
	ImGui::Text(lastSampleTimeText.c_str());

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Scene", 0);

	if (m_scene->materials.size() > 0 && ImGui::CollapsingHeader("Materials")) {
		for (Material* material : m_scene->materials) {
			if (ImGui::CollapsingHeader(material->name.c_str())) {
				if (ImGui::ColorEdit3("Albedo", (float*)&material->albedo)) {
					Reset();
				}
				if (ImGui::ColorEdit3("Emission", (float*)&material->emission)) {
					Reset();
				}
				if (ImGui::DragFloat("Roughness", &material->roughness, 0.01f, 0.0f, 1.0f)) {
					Reset();
				}
				if (ImGui::DragFloat("Metallic", &material->metallic, 0.01f, 0.0f, 1.0f)) {
					Reset();
				}
				if (ImGui::DragFloat("Transparency", &material->transparency, 0.01f, 0.0f, 1.0f)) {
					Reset();
				}
				if (ImGui::DragFloat("Refraction", &material->eta, 0.01f, 0.0f, 10.0f)) {
					Reset();
				}
			}
		}
	}

	ImGui::End();
}

void RayTracingRenderer::SetViewportSize(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	//m_rayTracer->SetResolution(resolution);
}