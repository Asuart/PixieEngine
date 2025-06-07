#include "pch.h"
#include "ParticleSimulation2D.h"

const char* UPDATE_HASHES_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_POSITIONS_BINDING) buffer positions_buffer {
   vec2 positions[];
};

layout(std430, binding = PREPROC_CELL_HASHES_BINDING) buffer cell_hashes_buffer {
   int cell_hashes[];
};

layout(std430, binding = PREPROC_CELL_INDEXES_BINDING) buffer cell_indexes_buffer {
   int cell_indexes[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const ivec2 cGridResolution = PREPROC_GRID_RESOLUTION;
const vec2 cCellSize = PREPROC_CELL_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;

	vec2 position = positions[index];
	ivec2 cell = ivec2(position / cCellSize);
	int hash = cell.y * cGridResolution.x + cell.x;
	cell_hashes[index] = hash; 
	cell_indexes[index] = int(index); 
}
)";

const char* SORT_HASHES_SHADER_SOURCE = R"(
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
    
    if (indexHigh >= cParticlesCount || indexLow >= cParticlesCount) {
        return;
    }
    
    int valueLow = cell_hashes[indexLow];
    int valueHigh = cell_hashes[indexHigh];
    
	int mask = int(valueLow > valueHigh);
	int temp = cell_indexes[indexLow];
	//cell_hashes[indexLow] = valueHigh * mask + valueLow * (1 - mask);
	//cell_hashes[indexHigh] = valueLow * mask +  valueHigh * (1 - mask);
	//cell_indexes[indexLow] = cell_indexes[indexHigh] * mask + temp * (1 - mask);
	//cell_indexes[indexHigh] = temp * mask + cell_indexes[indexHigh] * (1 - mask);
}
)";

const char* RESET_HASHES_STARTS_SHADER_SOURCE = R"(
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

const char* UPDATE_HASHES_STARTS_SHADER_SOURCE = R"(
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

const char* RESET_FORCES_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_FORCES_BINDING) buffer forces_buffer {
   vec2 forces[];
};

const float cGravity = PREPROC_GRAVITY;
const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const float cTimeStep = PREPROC_TIME_STEP;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;
	forces[index] = vec2(0, -PREPROC_GRAVITY) * cTimeStep;
}
)";

const char* CHECK_COLLISIONS_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_POSITIONS_BINDING) buffer positions_buffer {
   vec2 positions[];
};

layout(std430, binding = PREPROC_FORCES_BINDING) buffer forces_buffer {
   vec2 forces[];
};

layout(std430, binding = PREPROC_SIZES_BINDING) buffer sizes_buffer {
   float sizes[];
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
const float cTimeStep = PREPROC_TIME_STEP;
const ivec2 cGridResolution = PREPROC_GRID_RESOLUTION;
const vec2 cCellSize = PREPROC_CELL_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;

	vec2 position = positions[index];
	float size = sizes[index];
	vec2 acceleration = forces[index];

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
            if (node_index == index || node_index >= cParticlesCount) {
                continue;
            }
            vec2 offset = positions[node_index] - position;
            float distance = length(offset);
			float minDistance = size + sizes[node_index]; 
            if (distance >= minDistance) {
                continue;
            }
            vec2 normal = normalize(offset);
            float depth = minDistance - distance;
            acceleration -= normal * depth * 1000.0f * cTimeStep;
        }
	}

	forces[index] = acceleration;
}
)";

const char* UPDATE_POSITIONS_SHADER_SOURCE = R"(
#version 430 core

layout(local_size_x = PREPROC_LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = PREPROC_POSITIONS_BINDING) buffer positions_buffer {
   vec2 positions[];
};

layout(std430, binding = PREPROC_LAST_POSITIONS_BINDING) buffer last_positions_buffer {
   vec2 last_positions[];
};

layout(std430, binding = PREPROC_FORCES_BINDING) buffer forces_buffer {
   vec2 forces[];
};

const int cParticlesCount = PREPROC_PARTICLES_COUNT;
const float cVelocityDamping = PREPROC_STEP_DAMPING;
const float cTimeStep = PREPROC_TIME_STEP;
const vec2 cBoundsSize = PREPROC_BOUNDS_SIZE;

void main() {
	const uint index = gl_GlobalInvocationID.x;
	if(index >= cParticlesCount) return;

	vec2 acceleration = forces[index];
	vec2 current_position = positions[index];
	vec2 velocity = current_position - last_positions[index];
	vec2 new_position = current_position + (velocity * cVelocityDamping + acceleration * cTimeStep);
	vec2 last_position = positions[index];
	if(new_position.x < 0.0f) {
		last_position.x = new_position.x;
		new_position.x = -new_position.x;
	}
	else if(new_position.x >= cBoundsSize.x) {
		last_position.x = new_position.x;
		new_position.x = cBoundsSize.x + cBoundsSize.x - new_position.x;
	}	
	if(new_position.y < 0.0f) {
		last_position.y = new_position.y;
		new_position.y = -new_position.y;
	}
	else if(new_position.y >= cBoundsSize.y) {
		last_position.y = new_position.y;
		new_position.y = cBoundsSize.y - new_position.y;
	}	
	last_positions[index] = last_position;
	positions[index] = new_position;
}
)";


ParticleSimulation2D::ParticleSimulation2D() {}

bool ParticleSimulation2D::Init() {
	if (m_numParticles <= 0) {
		return false;
	}

	glGenBuffers(1, &m_positionsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positionsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_positions[0]), (GLvoid*)&m_positions[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_positionsBinding, m_positionsBuffer);

	glGenBuffers(1, &m_lastPositionsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lastPositionsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_lastPositions[0]), (GLvoid*)&m_lastPositions[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_lastPositionsBinding, m_lastPositionsBuffer);

	glGenBuffers(1, &m_forcesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_forcesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_forces[0]), (GLvoid*)&m_forces[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_forcesBinding, m_forcesBuffer);

	glGenBuffers(1, &m_massesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_massesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_masses[0]), (GLvoid*)&m_masses[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_massesBinding, m_massesBuffer);

	glGenBuffers(1, &m_sizesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_sizesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_sizes[0]), (GLvoid*)&m_sizes[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_sizesBinding, m_sizesBuffer);

	glGenBuffers(1, &m_cellHashesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cellHashesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_cellHashes[0]), (GLvoid*)&m_cellHashes[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_cellHashesBinding, m_cellHashesBuffer);

	glGenBuffers(1, &m_cellIndexesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cellIndexesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_cellIndexes[0]), (GLvoid*)&m_cellIndexes[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_cellIndexesBinding, m_cellIndexesBuffer);

	glGenBuffers(1, &m_cellStartsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cellStartsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_numParticles * sizeof(m_cellStarts[0]), (GLvoid*)&m_cellStarts[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_cellStartsBinding, m_cellStartsBuffer);

	if (m_numSprings > 0) {
		glGenBuffers(1, &m_springsStartsBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsStartsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_numSprings * sizeof(m_springsStarts[0]), (GLvoid*)&m_springsStarts[0], GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_springsStartsBinding, m_springsStartsBuffer);

		glGenBuffers(1, &m_springsEndsBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsEndsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_numSprings * sizeof(m_springsEnds[0]), (GLvoid*)&m_springsEnds[0], GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_springsEndsBinding, m_springsEndsBuffer);

		glGenBuffers(1, &m_springsLengthsBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsLengthsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_numSprings * sizeof(m_springsLengths[0]), (GLvoid*)&m_springsLengths[0], GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_springsLengthsBinding, m_springsLengthsBuffer);

		glGenBuffers(1, &m_springsStrengthsBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_springsStrengthsBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_numSprings * sizeof(m_springsStrengths[0]), (GLvoid*)&m_springsStrengths[0], GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_springsStrengthsBinding, m_springsStrengthsBuffer);
	}

	m_updateHashesShader = CompileComputeShader(UPDATE_HASHES_SHADER_SOURCE);
	m_sortHashesShader = CompileComputeShader(SORT_HASHES_SHADER_SOURCE);
	m_resetHashesStartsShader = CompileComputeShader(RESET_HASHES_STARTS_SHADER_SOURCE);
	m_updateHashesStartsShader = CompileComputeShader(UPDATE_HASHES_STARTS_SHADER_SOURCE);
	m_resetForcesShader = CompileComputeShader(RESET_FORCES_SHADER_SOURCE);
	m_checkCollisionsShader = CompileComputeShader(CHECK_COLLISIONS_SHADER_SOURCE);
	m_updatePositionsShader = CompileComputeShader(UPDATE_POSITIONS_SHADER_SOURCE);

	return true;
}

void ParticleSimulation2D::Step() {
	UpdateHashes();
	//FetchCellHashes();
	SortHashes();
	//FetchCellHashes();
	ResetHashesStarts();
	UpdateHashesStarts();
	ResetForces();
	UpdateSprings();
	CheckCollisions();
	UpdatePositions();
}

void ParticleSimulation2D::CreateParticle(glm::vec2 position, float size, float mass) {
	m_positions.push_back(position);
	m_lastPositions.push_back(position);
	m_forces.push_back(glm::vec2(0, 0));
	m_sizes.push_back(size);
	m_masses.push_back(mass);
	m_cellStarts.push_back(0);
	m_cellHashes.push_back(-1);
	m_cellIndexes.push_back(m_numParticles);
	m_numParticles++;
}

void ParticleSimulation2D::CreateSpring(uint32_t p0, uint32_t p1, float strength) {
	m_numSprings++;
	m_springsStarts.push_back(p0);
	m_springsEnds.push_back(p1);
	m_springsLengths.push_back(glm::length(m_positions[p0] - m_positions[p1]));
	m_springsStrengths.push_back(strength);
}

void ParticleSimulation2D::FetchPositions() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positionsBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numParticles * sizeof(m_cellHashes[0]), (GLvoid*)m_positions.data());
}

void ParticleSimulation2D::FetchCellHashes() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cellHashesBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numParticles * sizeof(m_cellHashes[0]), (GLvoid*)m_cellHashes.data());
}

void ParticleSimulation2D::UpdateHashes() {
	glUseProgram(m_updateHashesShader);
	glDispatchCompute(m_numParticles / m_localWorkGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::SortHashes() {
	int32_t numPairs = (int32_t)(pow(2.0f, ceil(log(m_numParticles) / log(2.0f))) / 2);
	int32_t numStages = (int32_t)log2f(numPairs * 2.0f);
	for (int32_t stageIndex = 0; stageIndex < numStages; stageIndex++) {
		for (int32_t stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++) {
			int32_t groupWidth = 1 << (stageIndex - stepIndex);
			int32_t groupHeight = 2 * groupWidth - 1;

			glUniform1i(glGetUniformLocation(m_sortHashesShader, "uGroupWidth"), groupWidth);
			glUniform1i(glGetUniformLocation(m_sortHashesShader, "uGroupHeight"), groupHeight);
			glUniform1i(glGetUniformLocation(m_sortHashesShader, "uStepIndex"), stepIndex);

			glUseProgram(m_sortHashesShader);
			glDispatchCompute(numPairs / m_localWorkGroupSize, 1, 1);
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
	glDispatchCompute(m_numParticles / m_localWorkGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::UpdateHashesStarts() {
	glUseProgram(m_updateHashesStartsShader);
	glDispatchCompute(m_numParticles / m_localWorkGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::ResetForces() {
	glUseProgram(m_resetForcesShader);
	glDispatchCompute(m_numParticles / m_localWorkGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::UpdateSprings() {
	//for (uint32_t springIndex = 0; springIndex < m_numSprings; springIndex++) {
	//	glm::vec3 offset = m_positions[m_springsStarts[springIndex]] - m_positions[m_springsEnds[springIndex]];
	//	float distance = glm::length(offset);
	//	glm::vec3 normal = glm::normalize(offset);
	//	float stretch = distance - m_springsLengths[springIndex];
	//	glm::vec3 force = normal * stretch * m_springsStrengths[springIndex] * m_substepDeltaTime;
	//	m_forces[m_springsStarts[springIndex]] -= force / m_masses[m_springsStarts[springIndex]];
	//	m_forces[m_springsEnds[springIndex]] += force / m_masses[m_springsEnds[springIndex]];
	//}
}

void ParticleSimulation2D::CheckCollisions() {
	glUseProgram(m_checkCollisionsShader);
	glDispatchCompute(m_numParticles / m_localWorkGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleSimulation2D::UpdatePositions() {
	glUseProgram(m_updatePositionsShader);
	glDispatchCompute(m_numParticles / m_localWorkGroupSize, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

GLuint ParticleSimulation2D::CompileComputeShader(const char* computeShaderSource) {
	std::string str = computeShaderSource;
	str = StringUtils::ReplaceAll(str, "PREPROC_POSITIONS_BINDING", std::to_string(m_positionsBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_LAST_POSITIONS_BINDING", std::to_string(m_lastPositionsBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_FORCES_BINDING", std::to_string(m_forcesBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_MASSES_BINDING", std::to_string(m_massesBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_SIZES_BINDING", std::to_string(m_sizesBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_HASHES_BINDING", std::to_string(m_cellHashesBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_INDEXES_BINDING", std::to_string(m_cellIndexesBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_STARTS_BINDING", std::to_string(m_cellStartsBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_STARTS_BINDING", std::to_string(m_springsStartsBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_ENDS_BINDING", std::to_string(m_springsEndsBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_LENGTHS_BINDING", std::to_string(m_springsLengthsBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_SPRINGS_STRENGTHS_BINDING", std::to_string(m_springsStrengthsBinding));
	str = StringUtils::ReplaceAll(str, "PREPROC_LOCAL_SIZE_X", std::to_string(m_localWorkGroupSize));
	str = StringUtils::ReplaceAll(str, "PREPROC_BOUNDS_SIZE", std::format("vec2({}, {})", m_bounds.x, m_bounds.y));
	str = StringUtils::ReplaceAll(str, "PREPROC_CELL_SIZE", std::format("vec2({}, {})", m_cellSize.x, m_cellSize.y));
	str = StringUtils::ReplaceAll(str, "PREPROC_GRID_RESOLUTION", std::format("ivec2({}, {})", m_gridResolution.x, m_gridResolution.y));
	str = StringUtils::ReplaceAll(str, "PREPROC_PARTICLES_COUNT", std::to_string(m_numParticles));
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
