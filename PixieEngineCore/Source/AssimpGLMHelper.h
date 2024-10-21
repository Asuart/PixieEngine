#pragma once
#include "pch.h"

class AssimpGLMHelpers {
public:
	static Mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
	static Vec2 GetGLMVec(const aiVector2D& vec);
	static Vec3 GetGLMVec(const aiVector3D& vec);
	static Quaternion GetGLMQuat(const aiQuaternion& pOrientation);
};
