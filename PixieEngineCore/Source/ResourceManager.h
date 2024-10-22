#pragma once
#include "pch.h"
#include "Scene.h"
#include "MeshAnimator.h"
#include "MeshAnimatorComponent.h"

enum class ResourceType : int32_t {
	Scene,
	Model,
	RGBTexture
};

class ResourceManager {
public:
	static void Initialize();
	static Scene* LoadScene(std::filesystem::path filePath);
	static bool SaveScene(std::filesystem::path filePath, Scene* scene);
	static SceneObject* LoadModel(std::filesystem::path filePath);
	static Texture<Vec3>* LoadRGBTexture(std::filesystem::path filePath);
	static Material* GetDefaultMaterial();

protected:
	static std::filesystem::path m_currentFilePath;
	static std::map<std::filesystem::path, Scene*> m_scenes;
	static std::map<std::filesystem::path, SceneObject*> m_Models;
	static std::map<std::filesystem::path, Texture<Vec3>*> m_RGBTextures;
	static std::map<std::string, Material*> m_materials;
	static std::vector<Mesh*> m_meshes;
	static Material* m_defaultMaterial;

	static bool CheckFileExtensionSupport(std::filesystem::path filePath, ResourceType type);
	static Scene* LoadPixieEngineScene(std::filesystem::path path);
	static Scene* LoadPBRTScene(std::filesystem::path filePath);
	static SceneObject* ProcessAssimpNode(const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap);
	static std::vector<Bone> ProcessAssimpAnimation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap);
	static SceneObject* ProcessAssimpMesh(const aiScene* scene, const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap);
	static Material* ProcessAssimpMaterial(const aiMaterial* material);
	static std::vector<Texture<Vec3>*> ProcessAssimpMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name);
};
