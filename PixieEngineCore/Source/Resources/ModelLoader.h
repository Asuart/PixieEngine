#pragma once
#include "pch.h"
#include "Scene/Scene.h"

class ModelLoader {
public:
	static void LoadModel(std::shared_ptr<Scene> scene, const std::filesystem::path& filePath);

protected:
	static SceneObject* ProcessAssimpNode(std::shared_ptr<Scene> currentScene, const std::filesystem::path& filePath, const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap);
	static std::vector<Bone> ProcessAssimpAnimation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap);
	static Mesh ProcessAssimpMesh(const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap);
	static std::shared_ptr<Material> ProcessAssimpMaterial(const std::filesystem::path& filePath, const aiMaterial* material);
	static std::vector<Texture> ProcessAssimpMaterialTextures(const std::filesystem::path& filePath, const aiMaterial* material, aiTextureType type, const std::string& name);

	// Assimp to GLM conversion helpers
	static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
	static glm::vec2 GetGLMVec(const aiVector2D& vec);
	static glm::vec3 GetGLMVec(const aiVector3D& vec);
	static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);
};
