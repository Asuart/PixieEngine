#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

class AssimpGLMHelpers {
public:
	static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
	static glm::vec3 GetGLMVec(const aiVector3D& vec);
	static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);
};
