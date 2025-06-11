#include "pch.h"
#include "ParticleSimulation2D.h"

static const char* UPDATE_HASHES_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_POSITIONS_BINDING) buffer positions_buffer {
   dvec2 positions[];
};

layout(std430, binding = PREPROC_CELL_HASHES_BINDING) buffer cell_hashes_buffer {
   int cell_hashes[];
};

layout(std430, binding = PREPROC_CELL_INDEXES_BINDING) buffer cell_indexes_buffer {
   int cell_indexes[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const ivec2 cGridResolution = PREPROC_GRID_RESOLUTION;
const dvec2 cCellSize = PREPROC_CELL_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;

	dvec2 position = positions[index];
	ivec2 cell = ivec2(position / cCellSize);
	int hash = cell.y * cGridResolution.x + cell.x;
	if(hash < 0 || hash >= cGridResolution.x * cGridResolution.y) {
		hash = -1;
	}
	cell_hashes[index] = hash; 
	cell_indexes[index] = int(index); 
}
)";

static const char* SORT_HASHES_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_CELL_HASHES_BINDING) buffer cell_hashes_buffer {
   int cell_hashes[];
};

layout(std430, binding = PREPROC_CELL_INDEXES_BINDING) buffer cell_indexes_buffer {
   int cell_indexes[];
};

const uint cParticlesCount = PREPROC_PARTICLES_COUNT;
uniform uint uGroupWidth;
uniform uint uGroupHeight;
uniform uint uStepIndex;

void main() {
    uint i = gl_GlobalInvocationID.x;
    uint h = i & (uGroupWidth - 1);
    uint indexLow = h + (uGroupHeight + 1) * (i / uGroupWidth);
    uint indexHigh = indexLow + (uStepIndex == 0 ? uGroupHeight - 2 * h : (uGroupHeight + 1) / 2);
    
    if (indexHigh >= cParticlesCount) {
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
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_CELL_STARTS_BINDING) buffer hashes_starts_buffer {
   int hashes_starts[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;
    hashes_starts[index] = -1;
}
)";

static const char* UPDATE_HASHES_STARTS_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_CELL_HASHES_BINDING) buffer cell_hashes_buffer {
   int cell_hashes[];
};

layout(std430, binding = PREPROC_CELL_STARTS_BINDING) buffer hashes_starts_buffer {
   int hashes_starts[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;

void main() {
	const int index = int(gl_GlobalInvocationID.x);
	if(index >= cParticlesCount) return;
    int key = cell_hashes[index];
    int keyPrev = (index == 0) ? -1 : cell_hashes[index - 1];
    if (key != keyPrev) {
        hashes_starts[key] = index;
    }
}
)";

static const char* RESET_FORCES_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_FORCES_BINDING) buffer forces_buffer {
   dvec2 forces[];
};

const double cGravity = PREPROC_GRAVITY;
const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const double cTimeStep = PREPROC_TIME_STEP;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;
	forces[index] = dvec2(0, -PREPROC_GRAVITY) * cTimeStep;
}
)";

static const char* CHECK_COLLISIONS_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_POSITIONS_BINDING) buffer positions_buffer {
   dvec2 positions[];
};

layout(std430, binding = PREPROC_FORCES_BINDING) buffer forces_buffer {
   dvec2 forces[];
};

layout(std430, binding = PREPROC_SIZES_BINDING) buffer sizes_buffer {
   double sizes[];
};

layout(std430, binding = PREPROC_CELL_HASHES_BINDING) buffer cell_hashes_buffer {
   int cell_hashes[];
};

layout(std430, binding = PREPROC_CELL_INDEXES_BINDING) buffer cell_indexes_buffer {
   int cell_indexes[];
};

layout(std430, binding = PREPROC_CELL_STARTS_BINDING) buffer hashes_starts_buffer {
   int hashes_starts[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const double cTimeStep = PREPROC_TIME_STEP;
const ivec2 cGridResolution = PREPROC_GRID_RESOLUTION;
const dvec2 cCellSize = PREPROC_CELL_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;

	dvec2 position = positions[index];
	double size = sizes[index];
	dvec2 acceleration = forces[index];

	ivec2 center_cell = ivec2(position / cCellSize);
	int neighbour_hashes[9];
	for(int y = -1, neighbour_index = 0; y <= 1; y++) {
		for(int x = -1; x <= 1; x++, neighbour_index++) {
			ivec2 cell = center_cell + ivec2(x, y);
			if(cell.x < 0 || cell.x >= cGridResolution.x || cell.y < 0 || cell.y >= cGridResolution.y) {
				neighbour_hashes[neighbour_index] = -1;
			}
			else {
				neighbour_hashes[neighbour_index] = cell.y * cGridResolution.x + cell.x;
			}
		}
	}
	
	for(int neighbour_index = 0; neighbour_index < 9; neighbour_index++) {
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
            dvec2 offset = positions[node_index] - position;
            double distance = length(offset);
			double minDistance = size + sizes[node_index]; 
            if (distance >= minDistance) {
                continue;
            }
            dvec2 normal = normalize(offset);
            double depth = minDistance - distance;
            acceleration -= normal * depth * 100.0f;
        }
	}

	forces[index] = acceleration;
}
)";

static const char* UPDATE_SPRINGS_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_POSITIONS_BINDING) buffer positions_buffer {
   dvec2 positions[];
};

layout(std430, binding = PREPROC_FORCES_BINDING) buffer forces_buffer {
   dvec2 forces[];
};

layout(std430, binding = PREPROC_MASSES_BINDING) buffer masses_buffer {
   double masses[];
};

layout(std430, binding = PREPROC_SPRINGS_STARTS_BINDING) buffer springs_starts_buffer {
   int springs_starts[];
};

layout(std430, binding = PREPROC_SPRINGS_ENDS_BINDING) buffer springs_ends_buffer {
   int springs_ends[];
};

layout(std430, binding = PREPROC_SPRINGS_LENGTHS_BINDING) buffer springs_lengths_buffer {
   double springs_lengths[];
};

layout(std430, binding = PREPROC_SPRINGS_STRENGTHS_BINDING) buffer springs_strengths_buffer {
   double springs_strengths[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const double cTimeStep = PREPROC_TIME_STEP;

uniform uint uStartIndex;
uniform uint uEndIndex;

void main() {
	const uint index = gl_GlobalInvocationID.x + uStartIndex;
	if(index >= uEndIndex) return;
	
	int start_index = springs_starts[index];
	int end_index = springs_ends[index];

	dvec2 offset = positions[start_index] - positions[end_index];
	double distance = length(offset);
	dvec2 normal = normalize(offset);
	double stretch = distance - springs_lengths[index];
	dvec2 force = normal * stretch * springs_strengths[index] * cTimeStep;
	forces[start_index] -= force / masses[start_index];
	forces[end_index] += force / masses[end_index];
}
)";

static const char* UPDATE_POSITIONS_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_POSITIONS_BINDING) buffer positions_buffer {
   dvec2 positions[];
};

layout(std430, binding = PREPROC_LAST_POSITIONS_BINDING) buffer last_positions_buffer {
   dvec2 last_positions[];
};

layout(std430, binding = PREPROC_FORCES_BINDING) buffer forces_buffer {
   dvec2 forces[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const double cVelocityDamping = PREPROC_STEP_DAMPING;
const double cTimeStep = PREPROC_TIME_STEP;
const dvec2 cBoundsSize = PREPROC_BOUNDS_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;

	dvec2 acceleration = forces[index];
	dvec2 current_position = positions[index];
	dvec2 velocity = current_position - last_positions[index];
	dvec2 new_position = current_position + (velocity * cVelocityDamping + acceleration * cTimeStep);
	dvec2 last_position = current_position;
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
	last_positions[index] = last_position;
	positions[index] = new_position;
}
)";

template<typename T>
void UploadGPUBufferData(GLuint buffer, const std::vector<T>& data, GLuint binding) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), (GLvoid*)&data[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);
}

ParticleSimulation2D::ParticleSimulation2D() {}

bool ParticleSimulation2D::Init() {
	if (m_initialized) return true;

	glGenBuffers(1, &m_positionsBuffer);
	glGenBuffers(1, &m_lastPositionsBuffer);
	glGenBuffers(1, &m_forcesBuffer);
	glGenBuffers(1, &m_massesBuffer);
	glGenBuffers(1, &m_sizesBuffer);
	glGenBuffers(1, &m_cellHashesBuffer);
	glGenBuffers(1, &m_cellIndexesBuffer);
	glGenBuffers(1, &m_cellStartsBuffer);
	glGenBuffers(1, &m_springsStartsBuffer);
	glGenBuffers(1, &m_springsEndsBuffer);
	glGenBuffers(1, &m_springsLengthsBuffer);
	glGenBuffers(1, &m_springsStrengthsBuffer);

	UploadData();

	m_initialized = true;

	return true;
}

void ParticleSimulation2D::Free() {
	if (!m_initialized) return;

	glDeleteBuffers(1, &m_positionsBuffer);
	glDeleteBuffers(1, &m_lastPositionsBuffer);
	glDeleteBuffers(1, &m_forcesBuffer);
	glDeleteBuffers(1, &m_massesBuffer);
	glDeleteBuffers(1, &m_sizesBuffer);
	glDeleteBuffers(1, &m_cellHashesBuffer);
	glDeleteBuffers(1, &m_cellIndexesBuffer);
	glDeleteBuffers(1, &m_cellStartsBuffer);
	glDeleteBuffers(1, &m_springsStartsBuffer);
	glDeleteBuffers(1, &m_springsEndsBuffer);
	glDeleteBuffers(1, &m_springsLengthsBuffer);
	glDeleteBuffers(1, &m_springsStrengthsBuffer);

	m_positionsBuffer = 0;
	m_lastPositionsBuffer = 0;
	m_forcesBuffer = 0;
	m_massesBuffer = 0;
	m_sizesBuffer = 0;
	m_cellHashesBuffer = 0;
	m_cellIndexesBuffer = 0;
	m_cellStartsBuffer = 0;
	m_springsStartsBuffer = 0;
	m_springsEndsBuffer = 0;
	m_springsLengthsBuffer = 0;
	m_springsStrengthsBuffer = 0;

	glDeleteProgram(m_updateHashesShader);
	glDeleteProgram(m_sortHashesShader);
	glDeleteProgram(m_resetHashesStartsShader);
	glDeleteProgram(m_updateHashesStartsShader);
	glDeleteProgram(m_resetForcesShader);
	glDeleteProgram(m_checkCollisionsShader);
	glDeleteProgram(m_updateSpringsShader);
	glDeleteProgram(m_updatePositionsShader);

	m_updateHashesShader = 0;
	m_sortHashesShader = 0;
	m_resetHashesStartsShader = 0;
	m_updateHashesStartsShader = 0;
	m_resetForcesShader = 0;
	m_checkCollisionsShader = 0;
	m_updateSpringsShader = 0;
	m_updatePositionsShader = 0;

	m_initialized = false;
}

void ParticleSimulation2D::UploadData() {
	if (m_positions.size() == 0) return;

	// Update grid resolution
	m_cellSize = glm::dvec2(m_largestParticle, m_largestParticle) * 2.0;
	m_gridResolution = glm::ivec2(glm::ceil(m_bounds / m_cellSize));
	m_cellStarts.resize(m_gridResolution.x * m_gridResolution.y);

	UploadGPUBufferData(m_positionsBuffer, m_positions, BufferBinding::Position);
	UploadGPUBufferData(m_lastPositionsBuffer, m_lastPositions, BufferBinding::LastPosition);
	UploadGPUBufferData(m_forcesBuffer, m_forces, BufferBinding::Forces);
	UploadGPUBufferData(m_massesBuffer, m_masses, BufferBinding::Masses);
	UploadGPUBufferData(m_sizesBuffer, m_sizes, BufferBinding::Sizes);
	UploadGPUBufferData(m_cellHashesBuffer, m_cellHashes, BufferBinding::CellHashes);
	UploadGPUBufferData(m_cellIndexesBuffer, m_cellIndexes, BufferBinding::CellIndexes);
	UploadGPUBufferData(m_cellStartsBuffer, m_cellStarts, BufferBinding::CellStarts);

	m_numParticlesOnGPU = m_positions.size();
	m_numCellStartsOnGPU = m_cellStarts.size();

	if (m_springsStarts.size() > 0) {
		ColorSprings();

		UploadGPUBufferData(m_springsStartsBuffer, m_springsStarts, BufferBinding::SpringsStarts);
		UploadGPUBufferData(m_springsEndsBuffer, m_springsEnds, BufferBinding::SpringsEnds);
		UploadGPUBufferData(m_springsLengthsBuffer, m_springsLengths, BufferBinding::SpringsLengths);
		UploadGPUBufferData(m_springsStrengthsBuffer, m_springsStrengths, BufferBinding::SpringsStrengths);

		m_numSpringsOnGPU = m_springsStarts.size();
	}

	CompileShaders();
}

void ParticleSimulation2D::ResetParticlesAndSprings() {
	m_largestParticle = 1.0f;
	m_positions.clear();
	m_lastPositions.clear();
	m_forces.clear();
	m_masses.clear();
	m_sizes.clear();
	m_cellHashes.clear();
	m_cellIndexes.clear();
	m_cellStarts.clear();
	m_springsStarts.clear();
	m_springsEnds.clear();
	m_springsLengths.clear();
	m_springsStrengths.clear();
}

void ParticleSimulation2D::Step() {
	UpdateHashes();
	SortHashes();
	ResetHashesStarts();
	UpdateHashesStarts();
	ResetForces();
	UpdateSprings();
	CheckCollisions();
	UpdatePositions();
}

void ParticleSimulation2D::SetBoundsSize(glm::dvec2 bounds) {
	m_bounds = bounds;
	m_cellSize = glm::dvec2(m_largestParticle, m_largestParticle) * 2.0;
	m_gridResolution = glm::ivec2(glm::ceil(m_bounds / m_cellSize));

	m_cellStarts.resize(m_gridResolution.x * m_gridResolution.y);

	UploadGPUBufferData(m_cellStartsBuffer, m_cellStarts, BufferBinding::CellStarts);

	m_numCellStartsOnGPU = m_cellStarts.size();

	CompileShaders();
}

void ParticleSimulation2D::CreateParticle(glm::dvec2 position, double size, double mass) {
	m_positions.push_back(position);
	m_lastPositions.push_back(position);
	m_forces.push_back(glm::dvec2(0, 0));
	m_sizes.push_back(size * 0.5f);
	m_masses.push_back(mass);
	m_cellHashes.push_back(-1);
	m_cellIndexes.push_back(m_positions.size() - 1);
	if (size > m_largestParticle) {
		m_largestParticle = size;
	}
}

void ParticleSimulation2D::CreateSpring(uint32_t p0, uint32_t p1, double strength) {
	m_springsStarts.push_back(p0);
	m_springsEnds.push_back(p1);
	m_springsLengths.push_back(glm::length(m_positions[p0] - m_positions[p1]));
	m_springsStrengths.push_back(strength);
}

std::vector<glm::dvec2> ParticleSimulation2D::FetchPositions() const {
	std::vector<glm::dvec2> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positionsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<glm::dvec2> ParticleSimulation2D::FetchLastPositions() const {
	std::vector<glm::dvec2> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lastPositionsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<glm::dvec2> ParticleSimulation2D::FetchForces() const {
	std::vector<glm::dvec2> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_forcesBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<double> ParticleSimulation2D::FetchMasses() const {
	std::vector<double> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_forcesBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<double> ParticleSimulation2D::FetchSizes() const {
	std::vector<double> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_sizesBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation2D::FetchCellHashes() const {
	std::vector<int32_t> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cellHashesBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation2D::FetchCellIndexes() const {
	std::vector<int32_t> buffer(m_numParticlesOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cellIndexesBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation2D::FetchCellStarts() const {
	std::vector<int32_t> buffer(m_numCellStartsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cellStartsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation2D::FetchSpringsStarts() const {
	std::vector<int32_t> buffer(m_numSpringsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsStartsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<int32_t> ParticleSimulation2D::FetchSpringsEnds() const {
	std::vector<int32_t> buffer(m_numSpringsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsEndsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<double> ParticleSimulation2D::FetchSpringsLengths() const {
	std::vector<double> buffer(m_numSpringsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsLengthsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

std::vector<double> ParticleSimulation2D::FetchSpringsStrengths() const {
	std::vector<double> buffer(m_numSpringsOnGPU);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsStrengthsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buffer.size() * sizeof(buffer[0]), (GLvoid*)buffer.data());
	return buffer;
}

void ParticleSimulation2D::UpdateHashes() {
	glUseProgram(m_updateHashesShader);
	int32_t dispatchSize = std::max(m_numParticlesOnGPU / m_localWorkGroupSize, 1);
	glDispatchCompute(dispatchSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::SortHashes() {
	glUseProgram(m_sortHashesShader);

	int32_t numPairs = (int32_t)(pow(2.0f, ceil(log(m_numParticlesOnGPU) / log(2.0f))) / 2);
	int32_t numStages = (int32_t)log2f(numPairs * 2.0f);
	for (int32_t stageIndex = 0; stageIndex < numStages; stageIndex++) {
		for (int32_t stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++) {
			int32_t groupWidth = 1 << (stageIndex - stepIndex);
			int32_t groupHeight = 2 * groupWidth - 1;

			glUniform1ui(glGetUniformLocation(m_sortHashesShader, "uGroupWidth"), groupWidth);
			glUniform1ui(glGetUniformLocation(m_sortHashesShader, "uGroupHeight"), groupHeight);
			glUniform1ui(glGetUniformLocation(m_sortHashesShader, "uStepIndex"), stepIndex);

			int32_t dispatchSize = std::max(numPairs / m_localWorkGroupSize, 1);
			glDispatchCompute(dispatchSize, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

			GLenum err;
			while ((err = glGetError()) != GL_NO_ERROR) {
				std::cout << err;
			}
		}
	}
}

void ParticleSimulation2D::ResetHashesStarts() {
	glUseProgram(m_resetHashesStartsShader);
	int32_t dispatchSize = std::max(m_numParticlesOnGPU / m_localWorkGroupSize + (m_numParticlesOnGPU % m_localWorkGroupSize ? 1 : 0), 1);
	glDispatchCompute(dispatchSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::UpdateHashesStarts() {
	glUseProgram(m_updateHashesStartsShader);
	int32_t dispatchSize = std::max(m_numParticlesOnGPU / m_localWorkGroupSize + (m_numParticlesOnGPU % m_localWorkGroupSize ? 1 : 0), 1);
	glDispatchCompute(dispatchSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::ResetForces() {
	glUseProgram(m_resetForcesShader);
	int32_t dispatchSize = std::max(m_numParticlesOnGPU / m_localWorkGroupSize + (m_numParticlesOnGPU % m_localWorkGroupSize ? 1 : 0), 1);
	glDispatchCompute(dispatchSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::UpdateSprings() {
	glUseProgram(m_updateSpringsShader);
	for (int32_t groupIndex = 0, startIndex = 0; groupIndex < m_springsGroups.size(); groupIndex++) {
		int32_t endIndex = startIndex + m_springsGroups[groupIndex];
		glUniform1ui(glGetUniformLocation(m_updateSpringsShader, "uStartIndex"), startIndex);
		glUniform1ui(glGetUniformLocation(m_updateSpringsShader, "uEndIndex"), endIndex);
		int32_t dispatchSize = std::max(m_springsGroups[groupIndex] / m_localWorkGroupSize + (m_springsGroups[groupIndex] % m_localWorkGroupSize ? 1 : 0), 1);
		glDispatchCompute(dispatchSize, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
		startIndex += m_springsGroups[groupIndex];
	}
}

void ParticleSimulation2D::CheckCollisions() {
	glUseProgram(m_checkCollisionsShader);
	int32_t dispatchSize = std::max(m_numParticlesOnGPU / m_localWorkGroupSize + (m_numParticlesOnGPU % m_localWorkGroupSize ? 1 : 0), 1);
	glDispatchCompute(dispatchSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::UpdatePositions() {
	glUseProgram(m_updatePositionsShader);
	int32_t dispatchSize = std::max(m_numParticlesOnGPU / m_localWorkGroupSize + (m_numParticlesOnGPU % m_localWorkGroupSize ? 1 : 0), 1);
	glDispatchCompute(dispatchSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

GLuint ParticleSimulation2D::CompileComputeShader(const char* computeShaderSource) {
	std::string str = computeShaderSource;
	str = StringUtils::ReplaceAll(str, "PREPROC_POSITIONS_BINDING", std::to_string(BufferBinding::Position));
	str = StringUtils::ReplaceAll(str, "PREPROC_LAST_POSITIONS_BINDING", std::to_string(BufferBinding::LastPosition));
	str = StringUtils::ReplaceAll(str, "PREPROC_FORCES_BINDING", std::to_string(BufferBinding::Forces));
	str = StringUtils::ReplaceAll(str, "PREPROC_MASSES_BINDING", std::to_string(BufferBinding::Masses));
	str = StringUtils::ReplaceAll(str, "PREPROC_SIZES_BINDING", std::to_string(BufferBinding::Sizes));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_HASHES_BINDING", std::to_string(BufferBinding::CellHashes));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_INDEXES_BINDING", std::to_string(BufferBinding::CellIndexes));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_STARTS_BINDING", std::to_string(BufferBinding::CellStarts));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_STARTS_BINDING", std::to_string(BufferBinding::SpringsStarts));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_ENDS_BINDING", std::to_string(BufferBinding::SpringsEnds));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_LENGTHS_BINDING", std::to_string(BufferBinding::SpringsLengths));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_STRENGTHS_BINDING", std::to_string(BufferBinding::SpringsStrengths));
	str = StringUtils::ReplaceAll(str, "PREPROC_LOCAL_SIZE_X", std::to_string(m_localWorkGroupSize));
	str = StringUtils::ReplaceAll(str, "PREPROC_BOUNDS_SIZE", std::format("dvec2({}, {})", m_bounds.x, m_bounds.y));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_SIZE", std::format("dvec2({}, {})", m_cellSize.x, m_cellSize.y));
	str = StringUtils::ReplaceAll(str, "PREPROC_GRID_RESOLUTION", std::format("ivec2({}, {})", m_gridResolution.x, m_gridResolution.y));
	str = StringUtils::ReplaceAll(str, "PREPROC_PARTICLES_COUNT", std::to_string(m_numParticlesOnGPU));
	str = StringUtils::ReplaceAll(str, "PREPROC_TIME_STEP", std::to_string(m_stepDeltaTime));
	str = StringUtils::ReplaceAll(str, "PREPROC_STEP_DAMPING", std::to_string(m_stepDamping));
	str = StringUtils::ReplaceAll(str, "PREPROC_GRAVITY", std::to_string(m_gravity));
	const char* preprocessedSource = str.c_str();

	GLint result = GL_FALSE;
	int32_t logLength;
	GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(ray_shader, 1, &preprocessedSource, NULL);
	glCompileShader(ray_shader);

	glGetShaderiv(ray_shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(ray_shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(ray_shader, logLength, NULL, &fragShaderError[0]);
	if (logLength) {
		std::cout << &fragShaderError[0] << std::endl;
	}

	GLuint rayProgram = glCreateProgram();
	glAttachShader(rayProgram, ray_shader);
	glLinkProgram(rayProgram);

	glGetProgramiv(rayProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(rayProgram, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(rayProgram, logLength, NULL, &programError[0]);
	if (logLength) {
		std::cout << &programError[0] << std::endl;
	}

	return rayProgram;
}

void ParticleSimulation2D::ColorSprings() {
	std::vector<std::vector<int32_t>> pointColors(m_positions.size());
	std::vector<std::vector<std::int32_t>> coloredSprings;

	for (int32_t springIndex = 0; springIndex < m_springsStarts.size(); springIndex++) {
		const std::vector<int32_t>& colors0 = pointColors[m_springsStarts[springIndex]];
		const std::vector<int32_t>& colors1 = pointColors[m_springsEnds[springIndex]];

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
		pointColors[m_springsStarts[springIndex]].push_back(color);
		pointColors[m_springsEnds[springIndex]].push_back(color);
	}

	std::vector<int32_t> sortedSpringsStarts;
	std::vector<int32_t> sortedSpringsEnds;
	std::vector<double> sortedSpringsLengths;
	std::vector<double> sortedSpringsStrengths;

	for (uint32_t colorIndex = 0; colorIndex < coloredSprings.size(); colorIndex++) {
		for (uint32_t springIndex = 0; springIndex < coloredSprings[colorIndex].size(); springIndex++) {
			sortedSpringsStarts.push_back(m_springsStarts[coloredSprings[colorIndex][springIndex]]);
			sortedSpringsEnds.push_back(m_springsEnds[coloredSprings[colorIndex][springIndex]]);
			sortedSpringsLengths.push_back(m_springsLengths[coloredSprings[colorIndex][springIndex]]);
			sortedSpringsStrengths.push_back(m_springsStrengths[coloredSprings[colorIndex][springIndex]]);
		}
	}

	m_springsStarts = sortedSpringsStarts;
	m_springsEnds = sortedSpringsEnds;
	m_springsLengths = sortedSpringsLengths;
	m_springsStrengths = sortedSpringsStrengths;

	m_springsGroups.clear();
	for (uint32_t colorIndex = 0; colorIndex < coloredSprings.size(); colorIndex++) {
		m_springsGroups.push_back((int32_t)coloredSprings[colorIndex].size());
	}
}

void ParticleSimulation2D::CompileShaders() {
	if (m_updateHashesShader) {
		glDeleteProgram(m_updateHashesShader);
	}
	m_updateHashesShader = CompileComputeShader(UPDATE_HASHES_SHADER_SOURCE);
	if (m_sortHashesShader) {
		glDeleteProgram(m_sortHashesShader);
	}
	m_sortHashesShader = CompileComputeShader(SORT_HASHES_SHADER_SOURCE);
	if (m_resetHashesStartsShader) {
		glDeleteProgram(m_resetHashesStartsShader);
	}
	m_resetHashesStartsShader = CompileComputeShader(RESET_HASHES_STARTS_SHADER_SOURCE);
	if (m_updateHashesStartsShader) {
		glDeleteProgram(m_updateHashesStartsShader);
	}
	m_updateHashesStartsShader = CompileComputeShader(UPDATE_HASHES_STARTS_SHADER_SOURCE);
	if (m_resetForcesShader) {
		glDeleteProgram(m_resetForcesShader);
	}
	m_resetForcesShader = CompileComputeShader(RESET_FORCES_SHADER_SOURCE);
	if (m_checkCollisionsShader) {
		glDeleteProgram(m_checkCollisionsShader);
	}
	m_checkCollisionsShader = CompileComputeShader(CHECK_COLLISIONS_SHADER_SOURCE);
	if (m_updateSpringsShader) {
		glDeleteProgram(m_updateSpringsShader);
	}
	m_updateSpringsShader = CompileComputeShader(UPDATE_SPRINGS_SHADER_SOURCE);
	if (m_updatePositionsShader) {
		glDeleteProgram(m_updatePositionsShader);
	}
	m_updatePositionsShader = CompileComputeShader(UPDATE_POSITIONS_SHADER_SOURCE);
}
