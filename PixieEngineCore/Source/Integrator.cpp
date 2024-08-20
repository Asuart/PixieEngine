#include "pch.h"
#include "Integrator.h"

void clear(std::queue<int32_t>& q) {
	std::queue<int32_t> empty;
	std::swap(q, empty);
}

Integrator::Integrator(const glm::ivec2& resolution)
	: Renderer(resolution), m_film(Film(resolution)), m_stats(resolution) {}

void Integrator::SetScene(Scene* scene) {
	bool wasRendering = m_isRendering;
	StopRender();
	m_scene = scene;
	Reset();
	if(wasRendering) StartRender();
}

void Integrator::SetResolution(const glm::ivec2& resolution) {
	bool wasRendering = m_isRendering;
	StopRender();
	m_resolution = resolution;
	m_film.Resize(resolution);
	m_stats.Resize(resolution);
	Reset();
	if (wasRendering) StartRender();
}

void Integrator::Reset() {
	bool wasRendering = m_isRendering;
	StopRender();
	m_film.Reset();
	m_stats.Clear();
	GenerateTiles();
	m_samples = 1;
	if (wasRendering) StartRender();
}

void Integrator::StartRender() {
	if (!m_scene || m_isRendering) return;
	m_isRendering = true;

	m_renderStartTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	m_sampleStartTime = m_renderStartTime;

	clear(m_tileQueue);
	for (size_t i = 0; i < m_tileQueue.size(); i++) {
		m_tileQueue.push((int32_t)i);
	}

	m_threadsCount = std::min(m_maxThreads, (int32_t)m_tiles.size());

	for (int32_t i = 0; i < m_threadsCount; i++) {
		m_renderThreads.push_back(new std::thread([&]() {
			while (m_isRendering) {
				m_tileQueueMutex.lock();
				if (m_tileQueue.empty()) {
					std::chrono::microseconds currrentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
					m_lastSampleTime = currrentTime - m_sampleStartTime;
					m_sampleStartTime = currrentTime;

					m_samples++;
					for (size_t i = 0; i < m_tiles.size(); i++) {
						m_tileQueue.push((int32_t)i);
					}
				}
				int32_t index = m_tileQueue.front();
				m_tileQueue.pop();
				m_tileQueueMutex.unlock();
				Bounds2i quad = m_tiles[index];
				Sampler sampler;
				for (int32_t y = quad.pMin.y; y < quad.pMax.y; y++) {
					for (int32_t x = quad.pMin.x; x < quad.pMax.x; x++) {
						PerPixel(x, y, &sampler);
					}
				}
			}
			}));
	}
}

void Integrator::StopRender() {
	if (!m_isRendering) return;
	m_isRendering = false;
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		m_renderThreads[i]->join();
	}
	for (int32_t i = 0; i < m_renderThreads.size(); i++) {
		delete m_renderThreads[i];
	}
	m_renderThreads.clear();
}

void Integrator::PerPixel(uint32_t x, uint32_t y, Sampler* sampler) {
	Vec2 uv = m_film.GetUV(x, y, sampler->Get2D());
	Ray ray = m_scene->GetMainCamera()->GetRay(x, y, uv);
	Spectrum color = Integrate(ray, sampler);
	m_film.AddPixel(x, y, glm::vec4(color.GetRGBValue(), 1.0));
}

bool Integrator::Unoccluded(uint32_t x, uint32_t y, const SurfaceInteraction& p0, const SurfaceInteraction& p1) {
	Vec3 dir = p1.position - p0.position;
	Float tMax = glm::length(dir);
	return !m_scene->IntersectP(Ray(x, y, p0.position, glm::normalize(dir)), m_stats, tMax - ShadowEpsilon);
}

void Integrator::GenerateTiles() {
	m_tiles.clear();
	glm::ivec2 tileGridSize = glm::ceil(Vec2(m_resolution) / Vec2(m_tileSize));
	for (int32_t yTile = 0; yTile < tileGridSize.y; yTile++) {
		for (int32_t xTile = 0; xTile < tileGridSize.x; xTile++) {
			int32_t xMax = glm::min(m_resolution.x, (xTile + 1) * m_tileSize.x);
			int32_t yMax = glm::min(m_resolution.y, (yTile + 1) * m_tileSize.y);
			glm::ivec2 min = glm::ivec2(xTile * m_tileSize.x, yTile * m_tileSize.y);
			glm::ivec2 max = glm::ivec2(xMax, yMax);
			m_tiles.push_back(Bounds2i(min, max));
		}
	}
}

uint32_t Integrator::GetThreadsCount() {
	return m_threadsCount;
}

uint32_t Integrator::GetSamplesCount() {
	return m_samples;
}

float Integrator::GetRenderTime() {
	std::chrono::milliseconds newTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return (newTime - m_renderStartTime).count() / 1000.0f;
}

float Integrator::GetLastSampleTime() {
	return m_lastSampleTime.count() / 1000.0f;
}