#pragma once
#include <filesystem>
#include "Scene.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "MeshAnimator.h"

class SceneLoader {
public:
	Scene* LoadScene(const std::string& sceneFilePath);

protected:
	SceneObject* ProcessNode(Scene* loadedScene, SceneObject* parentObject, std::map<std::string, BoneInfo>& boneInfoMap, aiNode* node, const aiScene* scene);
	Mesh* ProcessMesh(Scene* loadedScene, SceneObject* object, std::map<std::string, BoneInfo>& boneInfoMap, aiMesh* mesh, const aiScene* scene);
	Material* ProcessMaterial(Scene* loadedScene, uint32_t materialIndex, const aiScene* scene);
	void SetVertexBoneData(Vertex& vertex, int32_t boneID, Float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, std::map<std::string, BoneInfo>& boneInfoMap, aiMesh* mesh, const aiScene* scene);
	void LoadBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap, std::vector<Bone>& bones);
};
