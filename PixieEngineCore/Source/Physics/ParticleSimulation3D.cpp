#include "pch.h"
#include "ParticleSimulation3D.h"

static const char* UPDATE_HASHES_SHADER_SOURCE = R"(
const ivec3 cGridResolution = PREPROC_GRID_RESOLUTION;
const dvec3 cCellSize = PREPROC_CELL_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= PREPROC_PARTICLES_COUNT) return;

	dvec3 position = dvec3(positions_x[index], positions_y[index], positions_z[index]);
	ivec3 cell = ivec3(position / cCellSize);
	int hash = cell.z * cGridResolution.y * cGridResolution.x + cell.y * cGridResolution.x + cell.x;
	if(hash < 0 || hash >= cGridResolution.x * cGridResolution.y * cGridResolution.z) {
		hash = -1;
	}
	cell_hashes[index] = hash; 
	cell_indexes[index] = int(index); 
}
)";

static const char* SORT_HASHES_SHADER_SOURCE = R"(
uniform uint uGroupWidth;
uniform uint uGroupHeight;
uniform uint uStepIndex;

void main() {
    uint i = gl_GlobalInvocationID.x;
    uint h = i & (uGroupWidth - 1);
    uint indexLow = h + (uGroupHeight + 1) * (i / uGroupWidth);
    uint indexHigh = indexLow + (uStepIndex == 0 ? uGroupHeight - 2 * h : (uGroupHeight + 1) / 2);
    
    if (indexHigh >= PREPROC_PARTICLES_COUNT) {
        return;
    }
    
    int valueLow = cell_hashes[indexLow];
    int valueHigh = cell_hashes[indexHigh];
    
	if(valueLow > valueHigh) {
		cell_hashes[indexLow] = valueHigh;
		cell_hashes[indexHigh] = valueLow;
		int temp = cell_indexes[indexLow];
		cell_indexes[indexLow] = cell_indexes[indexHigh];
		cell_indexes[indexHigh] = temp;
	}
}
)";

static const char* RESET_HASHES_STARTS_SHADER_SOURCE = R"(
const ivec3 cGridResolution = PREPROC_GRID_RESOLUTION;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cGridResolution.x * cGridResolution.y * cGridResolution.z) return;
    hashes_starts[index] = -1;
}
)";

static const char* UPDATE_HASHES_STARTS_SHADER_SOURCE = R"(
void main() {
	const int index = int(gl_GlobalInvocationID.x);
	if(index >= PREPROC_PARTICLES_COUNT) return;
    int key = cell_hashes[index];
	if(key == -1) return;
    int keyPrev = (index == 0) ? -1 : cell_hashes[index - 1];
    if(key != keyPrev) {
        hashes_starts[key] = index;
    }
}
)";

static const char* RESET_FORCES_SHADER_SOURCE = R"(
void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= PREPROC_PARTICLES_COUNT) return;
	forces_x[index] = 0;
	forces_y[index] = -PREPROC_GRAVITY * PREPROC_TIME_STEP * masses[index];
	forces_z[index] = 0;
}
)";

static const char* CHECK_COLLISIONS_SHADER_SOURCE = R"(
const ivec3 cGridResolution = PREPROC_GRID_RESOLUTION;
const dvec3 cCellSize = PREPROC_CELL_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= PREPROC_PARTICLES_COUNT) return;

	dvec3 position = dvec3(positions_x[index], positions_y[index], positions_z[index]);
	double size = sizes[index];
	dvec3 force = dvec3(0, 0, 0);

	ivec3 center_cell = ivec3(position / cCellSize);
	int neighbour_hashes[27];
	for(int z = -1, neighbour_index = 0; z <= 1; z++) {
		for(int y = -1; y <= 1; y++) {
			for(int x = -1; x <= 1; x++, neighbour_index++) {
				ivec3 cell = center_cell + ivec3(x, y, z);
				if(cell.x < 0 || cell.x >= cGridResolution.x || cell.y < 0 || cell.y >= cGridResolution.y || cell.z < 0 || cell.z >= cGridResolution.z) {
					neighbour_hashes[neighbour_index] = -1;
				}
				else {
					neighbour_hashes[neighbour_index] = cell.z * cGridResolution.y * cGridResolution.x + cell.y * cGridResolution.x + cell.x;
				}
			}
		}
	}

	for(int neighbour_index = 0; neighbour_index < 27; neighbour_index++) {
		int cell_hash = neighbour_hashes[neighbour_index];
		if(cell_hash == -1) {
			continue;
		}
		int start = hashes_starts[cell_hash];
        if (start == -1) {
            continue;
        }
        for (int particle_index = start;; particle_index++) {
            if (cell_hashes[particle_index] != cell_hash) {
                break;
            }
            int node_index = cell_indexes[particle_index];
            if (node_index == index) {
                continue;
            }
			dvec3 otherPosition = dvec3(positions_x[node_index], positions_y[node_index], positions_z[node_index]);
            dvec3 offset = otherPosition - position;
            double distance = length(offset);
			double minDistance = size + sizes[node_index]; 
            if (distance >= minDistance) {
                continue;
            }
            dvec3 normal = normalize(offset);
            double depth = minDistance - distance;
            force += normal * depth * PREPROC_COLLISION_FORCE ;
        }
	}

	forces_x[index] += force.x;
	forces_y[index] += force.y;
	forces_z[index] += force.z;
}
)";

static const char* UPDATE_SPRINGS_SHADER_SOURCE = R"(
uniform uint uStartIndex;
uniform uint uEndIndex;

void main() {
	const uint index = gl_GlobalInvocationID.x + uStartIndex;
	if(index >= uEndIndex) return;
	
	int start_index = springs_starts[index];
	int end_index = springs_ends[index];

	dvec3 p0 = dvec3(positions_x[start_index], positions_y[start_index], positions_z[start_index]);
	dvec3 p1 = dvec3(positions_x[end_index], positions_y[end_index], positions_z[end_index]);

	dvec3 offset = p0 - p1;
	double distance = length(offset);
	dvec3 normal = normalize(offset);
	double stretch = distance - springs_lengths[index];
	dvec3 force = normal * stretch * springs_strengths[index] * PREPROC_TIME_STEP;

	forces_x[start_index] -= force.x;
	forces_y[start_index] -= force.y;
	forces_z[start_index] -= force.z;
	forces_x[end_index] += force.x;
	forces_y[end_index] += force.y;
	forces_z[end_index] += force.z;
}
)";

static const char* UPDATE_POSITIONS_SHADER_SOURCE = R"(
const dvec3 cBoundsSize = PREPROC_BOUNDS_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= PREPROC_PARTICLES_COUNT) return;

	dvec3 acceleration = dvec3(forces_x[index], forces_y[index], forces_z[index]) / masses[index];
	dvec3 current_position = dvec3(positions_x[index], positions_y[index], positions_z[index]);
	dvec3 last_position = dvec3(last_positions_x[index], last_positions_y[index], last_positions_z[index]);
	dvec3 velocity = current_position - last_position;
	dvec3 new_position = current_position + (velocity * PREPROC_STEP_DAMPING + acceleration * PREPROC_TIME_STEP);

	last_position = current_position;
	if(new_position.x < 0.0f) {
		new_position.x = -new_position.x;
		last_position.x = -current_position.x;
	}
	else if(new_position.x >= cBoundsSize.x) {
		new_position.x = cBoundsSize.x - (new_position.x - cBoundsSize.x);
		last_position.x = cBoundsSize.x - (current_position.x - cBoundsSize.x);
	}	
	if(new_position.y < 0.0f) {
		new_position.y = -new_position.y;
		last_position.y = -current_position.y;
	}
	else if(new_position.y >= cBoundsSize.y) {
		new_position.y = cBoundsSize.y - (new_position.y - cBoundsSize.y);
		last_position.y = cBoundsSize.y - (current_position.y - cBoundsSize.y);
	}	
	if(new_position.z < 0.0f) {
		new_position.z = -new_position.z;
		last_position.z = -current_position.z;
	}
	else if(new_position.z >= cBoundsSize.z) {
		new_position.z = cBoundsSize.z - (new_position.z - cBoundsSize.z);
		last_position.z = cBoundsSize.z - (current_position.z - cBoundsSize.z);
	}	

	last_positions_x[index] = last_position.x;
	last_positions_y[index] = last_position.y;
	last_positions_z[index] = last_position.z;
	positions_x[index] = new_position.x;
	positions_y[index] = new_position.y;
	positions_z[index] = new_position.z;
}
)";

ParticleSimulation3D::ParticleSimulation3D() {}

ParticleSimulation3D::~ParticleSimulation3D() {
	Free();
}

bool ParticleSimulation3D::Init() {
	if (m_initialized) return true;

	for (uint32_t bufferIndex = 0; bufferIndex < (int32_t)Buffer::Count; bufferIndex++) {
		glGenBuffers(1, &m_gpuBuffers[bufferIndex].id);
	}

	UploadData();

	m_initialized = true;
	return true;
}

void ParticleSimulation3D::Free() {
	if (!m_initialized) return;

	for (uint32_t bufferIndex = 0; bufferIndex < (int32_t)Buffer::Count; bufferIndex++) {
		glDeleteBuffers(1, &m_gpuBuffers[bufferIndex].id);
		m_gpuBuffers[bufferIndex].id = 0;
	}

	for (uint32_t programIndex = 0; programIndex < (int32_t)ShaderProgram::Count; programIndex++) {
		m_programs[programIndex] = ComputeShader(0);
	}

	m_initialized = false;
}

void ParticleSimulation3D::UploadData() {
	// Update grid resolution
	m_cellSize = glm::dvec3(m_largestParticle, m_largestParticle, m_largestParticle);
	m_gridResolution = glm::ivec3(glm::ceil(m_bounds / m_cellSize));
	m_cpuBuffers[(int32_t)Buffer::CellStarts].Resize(m_gridResolution.x * m_gridResolution.y);

	ColorSprings();

	for (uint32_t bufferIndex = 0; bufferIndex < (int32_t)Buffer::Count; bufferIndex++) {
		UploadBuffer((Buffer)bufferIndex);
	}

	m_numParticlesOnGPU = m_numParticlesOnCPU;
	m_numCellStartsOnGPU = m_gridResolution.x * m_gridResolution.y * m_gridResolution.z;
	m_numSpringsOnGPU = m_numSpringsOnCPU;

	CompileShaders();
}

void ParticleSimulation3D::ResetParticlesAndSprings() {
	m_largestParticle = 1.0f;
	m_numParticlesOnCPU = 0;
	m_numSpringsOnCPU = 0;
	for (uint32_t bufferIndex = 0; bufferIndex < (int32_t)Buffer::Count; bufferIndex++) {
		m_cpuBuffers[bufferIndex].Clear();
	}
}

void ParticleSimulation3D::Step() {
	UpdateHashes();
	SortHashes();
	ResetHashesStarts();
	UpdateHashesStarts();
	ResetForces();
	//UpdateSprings();
	CheckCollisions();
	UpdatePositions();
}

void ParticleSimulation3D::SetBoundsSize(glm::dvec3 bounds) {
	m_bounds = bounds;
	m_cellSize = glm::dvec3(m_largestParticle, m_largestParticle, m_largestParticle);
	m_gridResolution = glm::ivec3(glm::ceil(m_bounds / m_cellSize));

	m_cpuBuffers[(int32_t)Buffer::CellStarts].Resize(m_gridResolution.x * m_gridResolution.y * m_gridResolution.z);
	UploadBuffer(Buffer::CellStarts);
	m_numCellStartsOnGPU = m_gridResolution.x * m_gridResolution.y * m_gridResolution.z;

	CompileShaders();
}

void ParticleSimulation3D::CreateParticle(glm::dvec3 position, double size, double mass) {
	m_cpuBuffers[(int32_t)Buffer::PositionsX].Push<double>(position.x);
	m_cpuBuffers[(int32_t)Buffer::PositionsY].Push<double>(position.y);
	m_cpuBuffers[(int32_t)Buffer::PositionsZ].Push<double>(position.z);
	m_cpuBuffers[(int32_t)Buffer::LastPositionsX].Push<double>(position.x);
	m_cpuBuffers[(int32_t)Buffer::LastPositionsY].Push<double>(position.y);
	m_cpuBuffers[(int32_t)Buffer::LastPositionsZ].Push<double>(position.z);
	m_cpuBuffers[(int32_t)Buffer::ForcesX].Push<double>(0);
	m_cpuBuffers[(int32_t)Buffer::ForcesY].Push<double>(0);
	m_cpuBuffers[(int32_t)Buffer::ForcesZ].Push<double>(0);
	m_cpuBuffers[(int32_t)Buffer::Sizes].Push<double>(size * 0.5f);
	m_cpuBuffers[(int32_t)Buffer::Masses].Push<double>(mass);
	m_cpuBuffers[(int32_t)Buffer::CellHashes].Push<int32_t>(-1);
	m_cpuBuffers[(int32_t)Buffer::CellIndexes].Push<int32_t>(m_numParticlesOnCPU);
	if (size > m_largestParticle) {
		m_largestParticle = size;
	}
	m_numParticlesOnCPU++;
}

void ParticleSimulation3D::CreateSpring(int32_t p0, int32_t p1, double strength) {
	glm::dvec3 v0 = glm::dvec3(
		m_cpuBuffers[(int32_t)Buffer::PositionsX].Read<double>(p0),
		m_cpuBuffers[(int32_t)Buffer::PositionsY].Read<double>(p0),
		m_cpuBuffers[(int32_t)Buffer::PositionsZ].Read<double>(p0)
	);
	glm::dvec3 v1 = glm::dvec3(
		m_cpuBuffers[(int32_t)Buffer::PositionsX].Read<double>(p1),
		m_cpuBuffers[(int32_t)Buffer::PositionsY].Read<double>(p1),
		m_cpuBuffers[(int32_t)Buffer::PositionsZ].Read<double>(p1)
	);
	double length = glm::length(v0 - v1);

	m_cpuBuffers[(int32_t)Buffer::SpringsStarts].Push<int32_t>(p0);
	m_cpuBuffers[(int32_t)Buffer::SpringsEnds].Push<int32_t>(p1);
	m_cpuBuffers[(int32_t)Buffer::SpringsStrengths].Push<double>(strength);
	m_cpuBuffers[(int32_t)Buffer::SpringsLengths].Push<double>(length);

	m_numSpringsOnCPU++;
}

std::vector<glm::vec3> ParticleSimulation3D::FetchPositions() const {
	std::vector<double> x(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::PositionsX].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, x.size() * sizeof(x[0]), (GLvoid*)x.data());

	std::vector<double> y(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::PositionsY].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, y.size() * sizeof(y[0]), (GLvoid*)y.data());

	std::vector<double> z(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::PositionsZ].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, z.size() * sizeof(z[0]), (GLvoid*)z.data());

	std::vector<glm::vec3> result(m_numParticlesOnGPU);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = glm::vec3(x[i], y[i], z[i]);
	}
	return result;
}

std::vector<glm::vec3> ParticleSimulation3D::FetchLastPositions() const {
	std::vector<double> x(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::LastPositionsX].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, x.size() * sizeof(x[0]), (GLvoid*)x.data());

	std::vector<double> y(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::LastPositionsY].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, y.size() * sizeof(y[0]), (GLvoid*)y.data());

	std::vector<double> z(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::LastPositionsZ].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, z.size() * sizeof(z[0]), (GLvoid*)z.data());

	std::vector<glm::vec3> result(m_numParticlesOnGPU);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = glm::vec3(x[i], y[i], z[i]);
	}
	return result;
}

std::vector<glm::vec3> ParticleSimulation3D::FetchForces() const {
	std::vector<double> x(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::ForcesX].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, x.size() * sizeof(x[0]), (GLvoid*)x.data());

	std::vector<double> y(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::ForcesY].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, y.size() * sizeof(y[0]), (GLvoid*)y.data());

	std::vector<double> z(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::ForcesZ].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, z.size() * sizeof(z[0]), (GLvoid*)z.data());

	std::vector<glm::vec3> result(m_numParticlesOnGPU);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = glm::vec3(x[i], y[i], z[i]);
	}
	return result;
}

std::vector<float> ParticleSimulation3D::FetchMasses() const {
	std::vector<double> values(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::Masses].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, values.size() * sizeof(values[0]), (GLvoid*)values.data());

	std::vector<float> result(m_numParticlesOnGPU);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = (float)values[i];
	}
	return result;
}

std::vector<float> ParticleSimulation3D::FetchSizes() const {
	std::vector<double> values(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::Sizes].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, values.size() * sizeof(values[0]), (GLvoid*)values.data());

	std::vector<float> result(m_numParticlesOnGPU);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = (float)values[i];
	}
	return result;
}

std::vector<int32_t> ParticleSimulation3D::FetchCellHashes() const {
	std::vector<int32_t> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::CellHashes].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation3D::FetchCellIndexes() const {
	std::vector<int32_t> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::CellIndexes].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation3D::FetchCellStarts() const {
	std::vector<int32_t> buffer(m_numCellStartsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::CellStarts].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation3D::FetchSpringsStarts() const {
	std::vector<int32_t> buffer(m_numSpringsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::SpringsStarts].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation3D::FetchSpringsEnds() const {
	std::vector<int32_t> buffer(m_numSpringsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::SpringsEnds].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<float> ParticleSimulation3D::FetchSpringsLengths() const {
	std::vector<double> values(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::SpringsLengths].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, values.size() * sizeof(values[0]), (GLvoid*)values.data());

	std::vector<float> result(m_numParticlesOnGPU);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = (float)values[i];
	}
	return result;
}

std::vector<float> ParticleSimulation3D::FetchSpringsStrengths() const {
	std::vector<double> values(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)Buffer::SpringsStrengths].id);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, values.size() * sizeof(values[0]), (GLvoid*)values.data());

	std::vector<float> result(m_numParticlesOnGPU);
	for (size_t i = 0; i < result.size(); i++) {
		result[i] = (float)values[i];
	}
	return result;
}

void ParticleSimulation3D::UpdateHashes() {
	m_programs[(int32_t)ShaderProgram::UpdateHashes].Bind();
	glDispatchCompute(GetDispatchSizeX(m_numParticlesOnGPU), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation3D::SortHashes() {
	m_programs[(int32_t)ShaderProgram::SortHashes].Bind();

	int32_t numPairs = (int32_t)(pow(2.0f, ceil(log(m_numParticlesOnGPU) / log(2.0f))) / 2);
	int32_t numStages = (int32_t)log2f(numPairs * 2.0f);
	for (int32_t stageIndex = 0; stageIndex < numStages; stageIndex++) {
		for (int32_t stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++) {
			int32_t groupWidth = 1 << (stageIndex - stepIndex);
			int32_t groupHeight = 2 * groupWidth - 1;

			m_programs[(int32_t)ShaderProgram::SortHashes].SetUniform1ui("uGroupWidth", groupWidth);
			m_programs[(int32_t)ShaderProgram::SortHashes].SetUniform1ui("uGroupHeight", groupHeight);
			m_programs[(int32_t)ShaderProgram::SortHashes].SetUniform1ui("uStepIndex", stepIndex);

			glDispatchCompute(GetDispatchSizeX(numPairs), 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
		}
	}
}

void ParticleSimulation3D::ResetHashesStarts() {
	m_programs[(int32_t)ShaderProgram::ResetHashesStarts].Bind();
	glDispatchCompute(GetDispatchSizeX(m_numCellStartsOnGPU), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation3D::UpdateHashesStarts() {
	m_programs[(int32_t)ShaderProgram::UpdateHashesStarts].Bind();
	glDispatchCompute(GetDispatchSizeX(m_numParticlesOnGPU), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation3D::ResetForces() {
	m_programs[(int32_t)ShaderProgram::ResetForces].Bind();
	glDispatchCompute(GetDispatchSizeX(m_numParticlesOnGPU), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation3D::UpdateSprings() {
	m_programs[(int32_t)ShaderProgram::UpdateSprings].Bind();
	for (int32_t groupIndex = 0, startIndex = 0; groupIndex < m_springsGroups.size(); groupIndex++) {
		int32_t endIndex = startIndex + m_springsGroups[groupIndex];
		m_programs[(int32_t)ShaderProgram::UpdateSprings].SetUniform1ui("uStartIndex", startIndex);
		m_programs[(int32_t)ShaderProgram::UpdateSprings].SetUniform1ui("uEndIndex", endIndex);
		glDispatchCompute(GetDispatchSizeX(m_springsGroups[groupIndex]), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
		startIndex += m_springsGroups[groupIndex];
	}
}

void ParticleSimulation3D::CheckCollisions() {
	m_programs[(int32_t)ShaderProgram::CheckCollisions].Bind();
	glDispatchCompute(GetDispatchSizeX(m_numParticlesOnGPU), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation3D::UpdatePositions() {
	m_programs[(int32_t)ShaderProgram::UpdatePositions].Bind();
	glDispatchCompute(GetDispatchSizeX(m_numParticlesOnGPU), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

ComputeShader ParticleSimulation3D::CompileComputeShader(const std::string& computeShaderSource) {
	std::string preprocessedSource = PreprocessShader(computeShaderSource);
	return ShaderManager::CompileComputeShader(preprocessedSource.c_str());
}

std::string ParticleSimulation3D::PreprocessShader(const std::string& src) {
	const std::string shaderVersion = "#version 430 core\n\n";
	const std::string shaderWorkgroupSize = std::format("layout(local_size_x = {}, local_size_y = 1, local_size_z = 1) in;\n\n", m_localWorkGroupSize);

	const std::vector<std::array< std::string, 2>> bindingsData = {
		{ "positions_x", "double" },
		{ "positions_y", "double" },
		{ "positions_z", "double" },
		{ "last_positions_x", "double" },
		{ "last_positions_y", "double" },
		{ "last_positions_z", "double" },
		{ "forces_x", "double" },
		{ "forces_y", "double" },
		{ "forces_z", "double" },
		{ "masses", "double" },
		{ "sizes", "double" },
		{ "cell_hashes", "int" },
		{ "cell_indexes", "int" },
		{ "hashes_starts", "int" },
		{ "springs_starts", "int" },
		{ "springs_ends", "int" },
		{ "springs_lengths", "double" },
		{ "springs_strengths", "double" }
	};
	
	std::string assembledSource = shaderVersion + shaderWorkgroupSize;
	for (int32_t bufferIndex = 0; bufferIndex < (int32_t)Buffer::Count; bufferIndex++) {
		if (src.find(bindingsData[bufferIndex][0]) == -1) continue;
		std::string bufferCode = std::to_string(bufferIndex);
		std::string bindingCode = 
			std::format("layout(std430, binding = {}) buffer {}_buffer ", bufferCode, bindingsData[bufferIndex][0]) +
			"{\n" +
			std::format("{} {}[];", bindingsData[bufferIndex][1], bindingsData[bufferIndex][0]) +
			"\n};\n\n";
		assembledSource += bindingCode;
	}

	std::string preprocessed = src;
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_BOUNDS_SIZE", std::format("dvec3({}, {}, {})", m_bounds.x, m_bounds.y, m_bounds.z));
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_CELL_SIZE", std::format("dvec3({}, {}, {})", m_cellSize.x, m_cellSize.y, m_cellSize.z));
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_GRID_RESOLUTION", std::format("ivec3({}, {}, {})", m_gridResolution.x, m_gridResolution.y, m_gridResolution.z));
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_PARTICLES_COUNT", std::to_string(m_numParticlesOnGPU));
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_TIME_STEP", std::to_string(m_stepDeltaTime));
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_STEP_DAMPING", std::to_string(m_stepDamping));
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_GRAVITY", std::to_string(m_gravity));
	preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_COLLISION_FORCE", std::to_string(m_collisionForce));

	assembledSource += preprocessed;

	//std::cout << assembledSource << " \n\n\n";

	return assembledSource;
}

void ParticleSimulation3D::ColorSprings() {
	std::vector<std::vector<int32_t>> pointColors(m_numParticlesOnCPU);
	std::vector<std::vector<std::int32_t>> coloredSprings;

	for (int32_t springIndex = 0; springIndex < m_numSpringsOnCPU; springIndex++) {
		int32_t springStartIndex = m_cpuBuffers[(int32_t)Buffer::SpringsStarts].Read<int32_t>(springIndex);
		int32_t springEndIndex = m_cpuBuffers[(int32_t)Buffer::SpringsEnds].Read<int32_t>(springIndex);
		const std::vector<int32_t>& colors0 = pointColors[springStartIndex];
		const std::vector<int32_t>& colors1 = pointColors[springEndIndex];

		std::set<int32_t> usedColors;
		for (int32_t i = 0; i < colors0.size(); i++) {
			usedColors.insert(colors0[i]);
		}
		for (int32_t i = 0; i < colors1.size(); i++) {
			usedColors.insert(colors1[i]);
		}

		int32_t color = 0;
		while (usedColors.find(color) != usedColors.end()) {
			color++;
		}

		if (color >= coloredSprings.size()) {
			coloredSprings.resize(color + 1);
		}

		coloredSprings[color].push_back(springIndex);
		pointColors[springStartIndex].push_back(color);
		pointColors[springEndIndex].push_back(color);
	}

	std::vector<int32_t> sortedSpringsStarts;
	std::vector<int32_t> sortedSpringsEnds;
	std::vector<double> sortedSpringsLengths;
	std::vector<double> sortedSpringsStrengths;

	for (uint32_t colorIndex = 0; colorIndex < coloredSprings.size(); colorIndex++) {
		for (uint32_t springIndex = 0; springIndex < coloredSprings[colorIndex].size(); springIndex++) {
			int32_t index = coloredSprings[colorIndex][springIndex];
			sortedSpringsStarts.push_back(m_cpuBuffers[(int32_t)Buffer::SpringsStarts].Read<int32_t>(index));
			sortedSpringsEnds.push_back(m_cpuBuffers[(int32_t)Buffer::SpringsEnds].Read<int32_t>(index));
			sortedSpringsLengths.push_back(m_cpuBuffers[(int32_t)Buffer::SpringsLengths].Read<double>(index));
			sortedSpringsStrengths.push_back(m_cpuBuffers[(int32_t)Buffer::SpringsStrengths].Read<double>(index));
		}
	}

	m_cpuBuffers[(int32_t)Buffer::SpringsStarts] = sortedSpringsStarts;
	m_cpuBuffers[(int32_t)Buffer::SpringsEnds] = sortedSpringsEnds;
	m_cpuBuffers[(int32_t)Buffer::SpringsLengths] = sortedSpringsLengths;
	m_cpuBuffers[(int32_t)Buffer::SpringsStrengths] = sortedSpringsStrengths;

	m_springsGroups.clear();
	for (uint32_t colorIndex = 0; colorIndex < coloredSprings.size(); colorIndex++) {
		m_springsGroups.push_back((int32_t)coloredSprings[colorIndex].size());
	}
}

void ParticleSimulation3D::CompileShaders() {
	m_programs[(int32_t)ShaderProgram::UpdateHashes] = CompileComputeShader(UPDATE_HASHES_SHADER_SOURCE);
	m_programs[(int32_t)ShaderProgram::SortHashes] = CompileComputeShader(SORT_HASHES_SHADER_SOURCE);
	m_programs[(int32_t)ShaderProgram::ResetHashesStarts] = CompileComputeShader(RESET_HASHES_STARTS_SHADER_SOURCE);
	m_programs[(int32_t)ShaderProgram::UpdateHashesStarts] = CompileComputeShader(UPDATE_HASHES_STARTS_SHADER_SOURCE);
	m_programs[(int32_t)ShaderProgram::ResetForces] = CompileComputeShader(RESET_FORCES_SHADER_SOURCE);
	m_programs[(int32_t)ShaderProgram::CheckCollisions] = CompileComputeShader(CHECK_COLLISIONS_SHADER_SOURCE);
	m_programs[(int32_t)ShaderProgram::UpdateSprings] = CompileComputeShader(UPDATE_SPRINGS_SHADER_SOURCE);
	m_programs[(int32_t)ShaderProgram::UpdatePositions] = CompileComputeShader(UPDATE_POSITIONS_SHADER_SOURCE);
}

int32_t ParticleSimulation3D::GetDispatchSizeX(int32_t numInstances) {
	return numInstances / m_localWorkGroupSize + (numInstances % m_localWorkGroupSize ? 1 : 0);
}

void ParticleSimulation3D::UploadBuffer(Buffer buffer) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gpuBuffers[(int32_t)buffer].id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_cpuBuffers[(int32_t)buffer].Size(), (GLvoid*)m_cpuBuffers[(int32_t)buffer].Data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (int32_t)buffer, m_gpuBuffers[(int32_t)buffer].id);
	m_gpuBuffers[(int32_t)buffer].size = m_cpuBuffers[(int32_t)buffer].Size();
}
