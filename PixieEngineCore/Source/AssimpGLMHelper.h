#pragma once
#include "pch.h"

class AssimpGLMHelpers {
public:
	static Mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
	static Vec3 GetGLMVec(const aiVector3D& vec);
	static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);
};
