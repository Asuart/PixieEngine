#include "pch.h"
#include "RayTracingViewportWindow.h"
#include "PixieEngineApp.h"

void clear(std::queue<int32_t>& q) {
	std::queue<int32_t> empty;
	std::swap(q, empty);
}

RayTracingViewportWindow::RayTracingViewportWindow(PixieEngineApp& app, PixieEngineInterface& inter)
	: PixieEngineInterfaceWindow(app, inter),
	m_viewportCamera(Vec3(-10, 0,0), Vec3(0,0,0), Vec3(0, 1, 0), glm::radians(39.6f), 16.0f / 9.0f, 0, 10),
	m_viewportResolution(glm::ivec2(1280, 720)), m_cameraController(m_viewportCamera) {
	RayTracingStatistics::Resize(glm::ivec2(1280, 720));
	m_rayTracer = CreateRayTracer(m_rayTracingMode);
}

void RayTracingViewportWindow::Initialize() {
	m_film = new Film({ 1280, 720 });
	m_viewportFrameBuffer = new FrameBuffer(1280, 720);
	m_quadShader = CompileShader(QUAD_VERTEX_SHADER_SOURCE, QUAD_FRAGMENT_SHADER_SOURCE);
}

void RayTracingViewportWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("Ray Tracing Viewport")) {
		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
		if (glmViewportResolution != m_viewportResolution) {
			UpdateViewportResolution(glmViewportResolution);
		}

		if (ImGui::IsWindowFocused()) {
			if (m_cameraController.Update()) {
				Reset();
			}
			StartRender();
		}

		m_viewportFrameBuffer->Bind();
		glViewport(0, 0, m_viewportFrameBuffer->m_resolution.x, m_viewportFrameBuffer->m_resolution.y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (GetSamplesCount() <= 1) {
			Scene* scene = m_app.GetScene();
			if (scene) {
				m_app.GetDefaultRenderer()->DrawFrame(scene, &m_viewportCamera);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
		}
		
		glUseProgram(m_quadShader);
		GLuint posLoc = glGetUniformLocation(m_quadShader, "uPos");
		GLuint sizeLoc = glGetUniformLocation(m_quadShader, "uSize");
		GLuint samplesLoc = glGetUniformLocation(m_quadShader, "uSamples");
		
		float textureAspect = (float)m_film->m_resolution.x / m_film->m_resolution.y;
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
		
		glUniform2f(posLoc, posX, posY);
		glUniform2f(sizeLoc, sizeX, sizeY);
		
		glActiveTexture(GL_TEXTURE0);
		switch (m_visualizationMode) {
		case RayTracingVisualization::Integration:
			m_film->texture->Upload();
			glUniform1f(samplesLoc, (float)GetSamplesCount());
			m_film->texture->Bind(GL_TEXTURE0);
			break;
		case RayTracingVisualization::BoxChecksStatistics:
			RayTracingStatistics::UploadBoxTestsTextureLinear();
			glUniform1f(samplesLoc, 1.0f);
			RayTracingStatistics::BindBoxTestsTexture();
			break;
		case RayTracingVisualization::TriangleChecksStatistics:
			RayTracingStatistics::UploadTriangleTestsTextureLinear();
			glUniform1f(samplesLoc, 1.0f);
			RayTracingStatistics::BindTriangleTestsTexture();
			break;
		}
		
		ResourceManager::GetQuadMesh()->Draw();

		m_viewportFrameBuffer->Unbind();
		m_app.RestoreViewportSize();

		ImGui::Image((void*)(uint64_t)m_viewportFrameBuffer->m_texture, viewportResolution, { 0.0, 1.0 }, { 1.0, 0.0 });
	}
	else {
		StopRender();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

glm::ivec2 RayTracingViewportWindow::GetResolution() {
	return m_film->m_resolution;
}

void RayTracingViewportWindow::SetResolution(const glm::ivec2& resolution) {
	bool wasRendering = m_isRendering;
	if (m_isRendering) {
		StopRender();
	}
	m_viewportCamera.SetAspect((float)resolution.x / resolution.y);
	m_film->Resize(resolution);
	RayTracingStatistics::Resize(resolution);
	Reset();
	if (wasRendering) {
		StartRender();
	}
}

uint32_t RayTracingViewportWindow::GetMaxThreads() {
	return m_maxThreads;
}

void RayTracingViewportWindow::SetMaxThreads(int32_t maxThreads) {
	bool wasRendering = m_isRendering;
	StopRender();
	m_maxThreads = maxThreads;
	if (wasRendering) {
		StartRender();
	}
}

uint32_t RayTracingViewportWindow::GetThreadsCount() {
	return m_threadsCount;
}

void RayTracingViewportWindow::Reset() {
	bool wasRendering = m_isRendering;
	if (m_isRendering) {
		StopRender();
	}

	m_film->Reset();
	RayTracingStatistics::Reset();
	GenerateTiles();
	m_samples = 1;

	SceneSnapshot* sceneSnapshot = m_app.GetSceneSnapshot();
	if (!sceneSnapshot) {
		return;
	}

	Bounds3f sceneBounds = sceneSnapshot->GetBounds();
	const std::vector<DiffuseAreaLight*>& areaLights = sceneSnapshot->GetAreaLights();
	for (size_t i = 0; i < areaLights.size(); i++) {
		areaLights[i]->Preprocess(sceneBounds);
	}
	const std::vector<Light*>& lights = sceneSnapshot->GetInfiniteLights();
	for (size_t i = 0; i < lights.size(); i++) {
		lights[i]->Preprocess(sceneBounds);
	}

	if (wasRendering) {
		StartRender();
	}
}

void RayTracingViewportWindow::StartRender() {
	if (!m_app.GetSceneSnapshot() || m_isRendering) {
		return;
	}
	m_isRendering = true;

	m_renderStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	m_sampleStartTime = m_renderStartTime;

	clear(m_tileQueue);
	for (size_t i = 0; i < m_tiles.size(); i++) {
		m_tileQueue.push((int32_t)i);
	}

	m_threadsCount = std::min(m_maxThreads, (int32_t)m_tiles.size());

	m_spp = 32768, m_waveStart = 0, m_waveEnd = 1, m_nextWaveSize = 1;
	for (int32_t i = 0; i < m_threadsCount; i++) {
		m_renderThreads.push_back(new std::thread([&]() {
			Sampler* sampler = new IndependentSampler(m_spp);
			while (m_isRendering && m_waveStart < m_spp) {
				m_tileQueueMutex.lock();
				if (m_tileQueue.empty()) {
					std::chrono::microseconds currrentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
					m_lastSampleTime = currrentTime - m_sampleStartTime;
					m_sampleStartTime = currrentTime;

					m_samples += m_nextWaveSize;
					m_waveStart = m_waveEnd;
					m_waveEnd = std::min(m_spp, m_waveEnd + m_nextWaveSize);
					m_nextWaveSize = std::min(2 * m_nextWaveSize, m_maxWaveSize);

					for (size_t i = 0; i < m_tiles.size(); i++) {
						m_tileQueue.push((int32_t)i);
					}
				}
				int32_t index = m_tileQueue.front();
				m_tileQueue.pop();

				m_tileQueueMutex.unlock();
				Bounds2i quad = m_tiles[index];
				for (int32_t y = quad.min.y; y < quad.max.y; y++) {
					for (int32_t x = quad.min.x; x < quad.max.x; x++) {
						for (int32_t sampleIndex = m_waveStart; sampleIndex < m_waveEnd; sampleIndex++) {
							sampler->StartPixelSample(glm::ivec2(x, y), sampleIndex);
							PerPixel(x, y, sampler);
						}
					}
				}
			}
			delete sampler;
			}));
	}
}

void RayTracingViewportWindow::StopRender() {
	if (!m_isRendering) {
		return;
	}
	m_isRendering = false;
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		m_renderThreads[i]->join();
	}
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		delete m_renderThreads[i];
	}
	m_renderThreads.clear();
}

uint32_t RayTracingViewportWindow::GetSamplesCount() const {
	return m_samples;
}

float RayTracingViewportWindow::GetRenderTime() {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return (newTime - m_renderStartTime).count() / 1000.0f;
}

float RayTracingViewportWindow::GetLastSampleTime() {
	return m_lastSampleTime.count() / 1000.0f;
}

CameraSample RayTracingViewportWindow::GetCameraSample(uint32_t x, uint32_t y, const SampleFilter* filter, Sampler* sampler) {
	FilterSample fs = filter->Sample(sampler->GetPixel2D());
	return CameraSample(Vec2(x, y) + fs.p + Vec2(0.5f, 0.5f), fs.weight);
}

bool RayTracingViewportWindow::IsRendering() {
	return m_isRendering;
}

void RayTracingViewportWindow::SetVisualizationMode(RayTracingVisualization mode) {
	m_visualizationMode = mode;
}

void RayTracingViewportWindow::SetRayTracingMode(RayTracingMode mode) {
	bool wasRendering = m_isRendering;
	StopRender();
	if (m_rayTracer) {
		delete m_rayTracer;
	}
	m_rayTracingMode = mode;
	m_rayTracer = CreateRayTracer(mode);
	Reset();
	if (wasRendering) {
		StartRender();
	}
}

void RayTracingViewportWindow::GenerateTiles() {
	m_tiles.clear();
	glm::ivec2 resolution = m_film->m_resolution;
	glm::ivec2 tileGridSize = glm::ceil(Vec2(resolution) / Vec2(m_tileSize));
	for (int32_t yTile = 0; yTile < tileGridSize.y; yTile++) {
		for (int32_t xTile = 0; xTile < tileGridSize.x; xTile++) {
			int32_t xMax = glm::min(resolution.x, (xTile + 1) * m_tileSize.x);
			int32_t yMax = glm::min(resolution.y, (yTile + 1) * m_tileSize.y);
			glm::ivec2 min = glm::ivec2(xTile * m_tileSize.x, yTile * m_tileSize.y);
			glm::ivec2 max = glm::ivec2(xMax, yMax);
			m_tiles.push_back(Bounds2i(min, max));
		}
	}
}

void RayTracingViewportWindow::PerPixel(uint32_t x, uint32_t y, Sampler* sampler) {
	g_threadPixelCoordX = x;
	g_threadPixelCoordY = y;
	RayTracingStatistics::IncrementPixelSamples();

	SampleFilter* filter = m_film->GetFilter();
	CameraSample cameraSample = GetCameraSample(x, y, filter, sampler);
	Vec2 uv = m_film->GetUV(cameraSample.pFilm);
	Ray ray = m_viewportCamera.GetRay(uv);
	Spectrum L = m_rayTracer->SampleLightRay(m_app.GetSceneSnapshot(), ray, sampler);
	m_film->AddSample(x, y, L, cameraSample.filterWeight);
}

void RayTracingViewportWindow::UpdateViewportResolution(glm::ivec2 resolution) {
	bool wasRendering = m_isRendering;
	StopRender();
	m_viewportResolution = resolution;
	m_viewportFrameBuffer->Resize(resolution.x, resolution.y);
	m_film->Resize(resolution);
	m_viewportCamera.SetAspect((float)resolution.x / resolution.y);
	RayTracingStatistics::Resize(resolution);
	GenerateTiles();
	Reset();
	if (wasRendering) {
		StartRender();
	}
}