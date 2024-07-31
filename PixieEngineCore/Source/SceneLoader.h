#pragma once
#include "Scene.h"
#include "Primitive.h"
#include "Mesh.h"
#include "Scene.h"
#include "RTScene.h"

class SceneLoader {
public:
	RTScene* LoadScene(const std::string& sceneFilePath);
	void SaveScene(const std::string& sceneFileName, const Scene* scene);

private:
	RTScene* currentScene = nullptr;
	std::vector<Primitive*> shapePrimitives = std::vector<Primitive*>();
	int64_t trianglesCount = 0;
	int64_t invalidTrianglesCount = 0;

	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};
