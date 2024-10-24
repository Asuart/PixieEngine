#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <random>
#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <map>
#include <span>
#include <atomic>
#include <thread>
#include <optional>
#include <mutex>
#include <numeric>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "types.h"
#include "OpenGLInterface.h"

#ifdef PIXIE_ENGINE_DOUBLE_PRECISION
typedef GLdouble GLFloatType;
#else
typedef GLfloat GLFloatType;
#endif
