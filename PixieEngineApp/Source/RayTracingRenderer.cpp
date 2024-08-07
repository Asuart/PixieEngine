#include "RayTracingRenderer.h"

std::string to_string(RayTracingMode mode) {
	switch (mode) {
	case RayTracingMode::RandomWalk: return "Random Walk ";
	case RayTracingMode::SimplePathTracing: return "Simple Path Tracing";
	case RayTracingMode::PathTracing: return "Path Tracing";
	case RayTracingMode::TestNormals: return "Test Normals";
	case RayTracingMode::TestSampler: return "Test Sampler";
	default: return "Undefined Ray Tracing Mode";
	}
}

RayTracingRenderer::RayTracingRenderer(PixieEngineApp* parent, glm::ivec2 resolution, RTScene* scene)
	: m_parent(parent), m_resolution(resolution), m_rayTracer(new RandomWalkIntegrator(resolution)), m_scene(scene), m_viewportResolution(resolution) {
	SetScene(scene);
}

RayTracingRenderer::~RayTracingRenderer() {
	delete m_rayTracer;
}

void RayTracingRenderer::DrawFrame() {
	GLuint program = m_rayTracer->m_film.mesh->shader;
	GLuint posLoc = glGetUniformLocation(program, "uPos");
	GLuint sizeLoc = glGetUniformLocation(program, "uSize");
	GLuint samplesLoc = glGetUniformLocation(program, "uSamples");

	float textureAspect = (float)m_resolution.x / m_resolution.y;
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
	glUniform1f(samplesLoc, (float)m_rayTracer->GetSamplesCount());
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
	ImGui::Text("Ray Tracing Mode");
	if (ImGui::BeginCombo("##ray_tracing_mode", to_string(m_rayTracingMode).c_str())) {
		for (int n = 0; n < (int32_t)RayTracingMode::COUNT; n++) {
			RayTracingMode mode = RayTracingMode(n);
			bool isSelected = (m_rayTracingMode == mode);
			if (ImGui::Selectable(to_string(mode).c_str(), isSelected)) {
				SetRayTracingMode(mode);
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();

	ImGui::Text("Render Resolution");
	if (ImGui::InputInt2("##render_resolution", (int*)&m_resolution)) {
		m_rayTracer->SetResolution(m_resolution);
		for (Camera& camera : m_scene->cameras) {
			camera.aspect = (float)m_resolution.x / m_resolution.y;
		}
	}
	ImGui::Spacing();

	ImGui::Text("Max Ray Bounces");
	if (ImGui::InputInt("##max_ray_bounces", &m_rayTracer->m_maxDepth)) {
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

	ImGui::Text("Max Render Threads");
	if (ImGui::InputInt("##max_render_threads", &m_rayTracer->m_maxThreads)) {
		m_rayTracer->m_maxThreads = Clamp(m_rayTracer->m_maxThreads, 1, 128);
		m_rayTracer->StopRender();
		m_rayTracer->StartRender();
	}
	ImGui::Spacing();

	if (m_rayTracingMode == RayTracingMode::SimplePathTracing) {
		SimplePathIntegrator* integrator = dynamic_cast<SimplePathIntegrator*>(m_rayTracer);
		if (ImGui::Checkbox("Sample Lights", &integrator->m_sampleLights)) {
			Reset();
		}
		if (ImGui::Checkbox("Sample BSDF", &integrator->m_sampleBSDF)) {
			Reset();
		}
		ImGui::Spacing();
	} 
	else if (m_rayTracingMode == RayTracingMode::PathTracing) {
		PathIntegrator* integrator = dynamic_cast<PathIntegrator*>(m_rayTracer);
		if (ImGui::Checkbox("Regularize BSDF", &integrator->m_regularize)) {
			Reset();
		}
		ImGui::Spacing();
	}

	std::string threadsText = std::string("Threads: ") + std::to_string(m_rayTracer->GetThreadsCount());
	ImGui::Text(threadsText.c_str());
	
	std::string samplesText = std::string("Samples: ") + std::to_string(m_rayTracer->GetSamplesCount());
	ImGui::Text(samplesText.c_str());
	
	std::string renderTimeText = std::string("Render Time: ") + std::to_string(m_rayTracer->GetRenderTime());
	ImGui::Text(renderTimeText.c_str());
	
	std::string lastSampleTimeText = std::string("Last Sample Time: ") + std::to_string(m_rayTracer->GetLastSampleTime());
	ImGui::Text(lastSampleTimeText.c_str());
}

void RayTracingRenderer::SetViewportSize(glm::ivec2 resolution) {
	m_viewportResolution = resolution;
	if (m_resizeRendererToVieport) {
		m_rayTracer->SetResolution(resolution);
	}
}

void RayTracingRenderer::SetRayTracingMode(RayTracingMode mode) {
	if (mode == m_rayTracingMode) return;
	m_rayTracingMode = mode;

	if (m_rayTracer) {
		m_rayTracer->StopRender();
		delete m_rayTracer;
	}

	switch (m_rayTracingMode) {
	case RayTracingMode::SimplePathTracing: m_rayTracer = new SimplePathIntegrator(m_resolution); break;
	case RayTracingMode::PathTracing: m_rayTracer = new PathIntegrator(m_resolution); break;
	case RayTracingMode::TestNormals: m_rayTracer = new TestNormalsIntegrator(m_resolution); break;
	case RayTracingMode::TestSampler: m_rayTracer = new TestSamplerIntegrator(m_resolution); break;
	case RayTracingMode::RandomWalk: default:
		m_rayTracer = new RandomWalkIntegrator(m_resolution); break;
	}

	m_rayTracer->SetScene(m_scene);
	m_rayTracer->StartRender();
}
