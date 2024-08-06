#pragma once
#include <filesystem>
#include "Scene.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "MeshAnimator.h"
#include "DiffuseMaterial.h"
#include "DielectricMaterial.h"
#include "ColorTexture.h"

class SceneLoader {
public:
	Scene* LoadScene(const std::string& sceneFilePath);

protected:
	SceneObject* ProcessNode(Scene* loadedScene, SceneObject* parentObject, aiNode* node, const aiScene* scene);
	Mesh* ProcessMesh(Scene* loadedScene, SceneObject* object, aiMesh* mesh, const aiScene* scene);
	Material* ProcessMaterial(Scene* loadedScene, uint32_t materialIndex, const aiScene* scene);
	void SetVertexBoneData(Vertex& vertex, int32_t boneID, Float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	void LoadBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap);
};
