
#include "SceneLoader.h"

void SceneLoader::SaveScene(const std::string& sceneFileName, const Scene* scene) {

}

RTScene* SceneLoader::LoadScene(const std::string& filePath) {
	RTScene* newScene = new RTScene();
	currentScene = newScene;
	std::cout << "Loading scene from file: " << filePath << "\n";

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
		return newScene;
	}

	ProcessNode(scene->mRootNode, scene);
	currentScene->rootPrimitive = new BVHAggregate(shapePrimitives);

	std::cout << "Scene triangles count: " << trianglesCount << ", invalid triangles count: " << invalidTrianglesCount << "\n";

	trianglesCount = 0;
	invalidTrianglesCount = 0;
	currentScene = nullptr;
	shapePrimitives.clear();

	return newScene;
}

void SceneLoader::ProcessNode(aiNode* node, const aiScene* scene) {
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		//currentObject->AddChild(new SceneObject(node->mChildren[i]->mName.C_Str()));
		//currentObject = currentObject->children[i];
		ProcessNode(node->mChildren[i], scene);
		//currentObject = currentObject->parent;
	}
}

void SceneLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	std::cout << "  Mesh: " << mesh->mName.C_Str() << " (vertices: " << mesh->mNumVertices << ", faces: " << mesh->mNumFaces << ")\n";

	RTMaterial* mat = nullptr;

	if (mesh->mMaterialIndex >= 0) {
		std::string materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
		std::cout << "  Material: " << materialName << "\n";

		for (RTMaterial* material : currentScene->materials) {
			if (material->name == materialName) {
				mat = material;
				break;
			}
		}

		if (!mat) {

			aiColor4D color, colorEmissive, emissionIntensity;
			Float emissionInt, roughness;
			aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color);
			aiGetMaterialFloat(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_EMISSIVE_INTENSITY, &emissionInt);
			aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_EMISSIVE_INTENSITY, &emissionIntensity);
			aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_EMISSIVE, &colorEmissive);
			aiGetMaterialFloat(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_ROUGHNESS_FACTOR, &roughness);

			if (std::string(scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()) == std::string("Glass")) {
				mat = new DielectricMaterial(materialName, 0.0f, 0.0f, 1.7f, true);
				mat->transparency = 1.0f;
				mat->eta = 1.7f;
			}
			else {
				mat = new DiffuseMaterial(materialName, new ColorTexture(Vec3(color.r, color.g, color.b)));
			}
			mat->albedo = Vec3(color.r, color.g, color.b);
			mat->texture = new ColorTexture(Vec3(color.r, color.g, color.b));
			mat->emission = Vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);

			currentScene->materials.push_back(mat);
		}
	}

	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3) {
			std::cout << "	ERROR: mesh has not triangulated faces.\n";
			break;
		}
		Vec3 p0 = Vec3(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z);
		Vec3 p1 = Vec3(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z);
		Vec3 p2 = Vec3(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z);
		Shape* triangle = new CachedTriangle(p0, p1, p2);
		if (!triangle->IsValid()) {
			delete triangle;
			invalidTrianglesCount++;
			continue;
		}
		currentScene->shapes.push_back(triangle);
		shapePrimitives.push_back(new ShapePrimitive(triangle, mat));
		if (mat->emission != Vec3(0)) {
			currentScene->lights.push_back(DiffuseAreaLight(triangle, mat->emission * 10.0f));
		}
		trianglesCount++;
	}

	//ExtractBoneWeightForVertices(objectMesh.vertices, mesh, scene);
}

void SceneLoader::SetVertexBoneData(Vertex& vertex, int boneID, Float weight) {
	for (uint32_t i = 0; i < MaxBonesPerVertex; i++) {
		if (vertex.boneIDs[i] < 0) {
			vertex.boneWeights[i] = weight;
			vertex.boneIDs[i] = boneID;
			break;
		}
	}
}

void SceneLoader::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
	/*
	for (int32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int32_t boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (currentObject->boneInfoMap.find(boneName) == currentObject->boneInfoMap.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = currentObject->boneCounter;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			currentObject->boneInfoMap[boneName] = newBoneInfo;
			boneID = currentObject->boneCounter;
			currentObject->boneCounter++;
		}
		else {
			boneID = currentObject->boneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int32_t numWeights = mesh->mBones[boneIndex]->mNumWeights;
		for (int32_t weightIndex = 0; weightIndex < numWeights; weightIndex++) {
			int32_t vertexId = weights[weightIndex].mVertexId;
			Float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
	*/
}