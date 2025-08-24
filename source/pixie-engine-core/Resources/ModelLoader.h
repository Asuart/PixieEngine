#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Rendering/MeshHandle.h"
#include "Rendering/TextureHandle.h"
#include "Scene/Scene.h"

namespace PixieEngine {

class ModelLoader {
public:
	static void LoadModel(std::shared_ptr<Scene> scene, const std::filesystem::path& filePath);
	static Mesh LoadMesh(const std::filesystem::path& filePath);

protected:
	static SceneObject* ProcessAssimpNode(std::shared_ptr<Scene> currentScene, const std::filesystem::path& filePath, const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap);
	static std::vector<Bone> ProcessAssimpAnimation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap);
	static Mesh ProcessAssimpMesh(const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap);
	static std::shared_ptr<Material> ProcessAssimpMaterial(const std::filesystem::path& filePath, const aiMaterial* material);
	static std::vector<TextureHandle> ProcessAssimpMaterialTextures(const std::filesystem::path& filePath, const aiMaterial* material, aiTextureType type, const std::string& name);
};

}