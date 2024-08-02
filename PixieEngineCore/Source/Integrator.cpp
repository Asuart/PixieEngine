#include "Integrator.h"

void clear(std::queue<int32_t>& q) {
	std::queue<int32_t> empty;
	std::swap(q, empty);
}

Integrator::Integrator(const glm::ivec2& resolution)
	: Renderer(resolution), m_film(Film(resolution)), m_rayStatBuffer(resolution),
	m_boxCheckStatBuffer(resolution), m_triangleCheckStatBuffer(resolution), m_sampleCountBuffer(resolution) {}

void Integrator::SetScene(RTScene* scene) {
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
	m_rayStatBuffer.Resize(resolution);
	m_boxCheckStatBuffer.Resize(resolution);
	m_triangleCheckStatBuffer.Resize(resolution);
	m_sampleCountBuffer.Resize(resolution);
	Reset();
	if (wasRendering) StartRender();
}

void Integrator::Reset() {
	bool wasRendering = m_isRendering;
	StopRender();
	m_film.Reset();
	m_rayStatBuffer.Clear();
	m_boxCheckStatBuffer.Clear();
	m_triangleCheckStatBuffer.Clear();
	m_sampleCountBuffer.Clear();
	GenerateTiles();
	m_samples = 1;
	if (wasRendering) StartRender();
}

void Integrator::StartRender() {
	if (!m_scene || m_isRendering) return;
	m_isRendering = true;

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
					//std::chrono::microseconds currrentTime = Time::GetTime();
					//renderTime = (currrentTime - startTime).count() / 1000.0;
					//lastSampleTime = (currrentTime - sampleStartTime).count() / 1000.0;
					//sampleStartTime = currrentTime;
					m_samples++;
					for (size_t i = 0; i < m_tiles.size(); i++) {
						m_tileQueue.push((int32_t)i);
					}
				}
				int32_t index = m_tileQueue.front();
				m_tileQueue.pop();
				m_tileQueueMutex.unlock();
				Bounds2i quad = m_tiles[index];
				for (int32_t y = quad.pMin.y; y < quad.pMax.y; y++) {
					for (int32_t x = quad.pMin.x; x < quad.pMax.x; x++) {
						PerPixel(x, y);
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

void Integrator::PerPixel(uint32_t x, uint32_t y) {
	Vec2 uv = m_film.GetUV(x, y, Vec2(RandomFloat(), RandomFloat()));
	Ray ray = m_scene->mainCamera->GetRay(uv);
	Vec3 color = Integrate(ray);
	m_film.AddPixel(x, y, glm::vec4(color, 1.0));
}

bool Integrator::Unoccluded(const RTInteraction& p0, const RTInteraction& p1) const {
	Vec3 dir = p1.p - p0.p;
	Float tMax = glm::length(dir);
	return !m_scene->IntersectP(Ray(p0.p, glm::normalize(dir)), tMax - ShadowEpsilon);
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