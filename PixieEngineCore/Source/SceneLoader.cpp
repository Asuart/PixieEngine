#include "SceneLoader.h"

Scene* SceneLoader::LoadScene(const std::string& filePath) {
	std::cout << "Loading scene from file: " << filePath << "\n";
	Scene* loadedScene = new Scene(std::filesystem::path(filePath).filename().string());

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
		return loadedScene;
	}

	std::map<std::string, BoneInfo> boneInfoMap;

	SceneObject* rootObject = ProcessNode(loadedScene, loadedScene->rootObject, scene->mRootNode, scene);
	loadedScene->AddObject(rootObject);

	std::vector<Animation*> animations;
	for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
		LoadBones(scene->mAnimations[i], boneInfoMap);
		animations.push_back(new Animation(scene->mAnimations[i]->mDuration, scene->mAnimations[i]->mTicksPerSecond, rootObject));
	}

	return loadedScene;
}

SceneObject* SceneLoader::ProcessNode(Scene* loadedScene, SceneObject* parentObject, aiNode* node, const aiScene* scene) {
	std::cout << "  Node: " << node->mName.C_Str() << " (children: " << node->mNumChildren << ", meshes: " << node->mNumMeshes << ")\n";
	SceneObject* nodeObject = new SceneObject(node->mName.data, Transform(AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation)));

	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		Mesh* mesh = ProcessMesh(loadedScene, nodeObject, scene->mMeshes[node->mMeshes[i]], scene);
		loadedScene->meshes.push_back(mesh);
		nodeObject->AddComponent(new MeshComponent(mesh, nodeObject));
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		SceneObject* child = ProcessNode(loadedScene, nodeObject, node->mChildren[i], scene);
		loadedScene->AddObject(child, parentObject);
	}
}

Mesh* SceneLoader::ProcessMesh(Scene* loadedScene, SceneObject* object, aiMesh* mesh, const aiScene* scene) {
	std::cout << "  Mesh: " << mesh->mName.C_Str() << " (vertices: " << mesh->mNumVertices << ", faces: " << mesh->mNumFaces << ")\n";

	Material* material = nullptr;
	if (mesh->mMaterialIndex >= 0) {
		std::string materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
		std::cout << "  Material: " << materialName << "\n";
		for (Material* existingMaterial : loadedScene->materials) {
			if (existingMaterial->name == materialName) {
				material = existingMaterial;
				break;
			}
		}
		if (!material) {
			material = ProcessMaterial(loadedScene, mesh->mMaterialIndex, scene);
			loadedScene->materials.push_back(material);
		}
	}
	object->AddComponent(new MaterialComponent(material, object));

	std::vector<Vertex> vertices;
	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
		vertices.push_back(Vertex(
			{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z },
			mesh->HasNormals() ? Vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : Vec3(0.0f),
			mesh->HasTextureCoords(0) ? Vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : Vec2(0.0f)
		));
	}

	std::vector<int32_t> indices;
	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int32_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	return new Mesh(vertices, indices);
}

Material* SceneLoader::ProcessMaterial(Scene* loadedScene, uint32_t materialIndex, const aiScene* scene) {
	aiColor4D color, colorEmissive, emissionIntensity;
	float emissionInt, roughness;
	aiGetMaterialColor(scene->mMaterials[materialIndex], AI_MATKEY_COLOR_DIFFUSE, &color);
	aiGetMaterialFloat(scene->mMaterials[materialIndex], AI_MATKEY_EMISSIVE_INTENSITY, &emissionInt);
	aiGetMaterialColor(scene->mMaterials[materialIndex], AI_MATKEY_EMISSIVE_INTENSITY, &emissionIntensity);
	aiGetMaterialColor(scene->mMaterials[materialIndex], AI_MATKEY_COLOR_EMISSIVE, &colorEmissive);
	aiGetMaterialFloat(scene->mMaterials[materialIndex], AI_MATKEY_ROUGHNESS_FACTOR, &roughness);

	Material* material = nullptr;
	std::string materialName = scene->mMaterials[materialIndex]->GetName().C_Str();
	if (materialName =="Glass") {
		material = new DielectricMaterial(materialName, 0.0f, 0.0f, 1.7f, true);
		material->transparency = 1.0f;
		material->eta = 1.7f;
	}
	else {
		material = new DiffuseMaterial(materialName, new ColorTexture(Vec3(color.r, color.g, color.b)));
	}
	material->albedo = Vec3(color.r, color.g, color.b);
	material->rtTexture = new ColorTexture(Vec3(color.r, color.g, color.b));
	material->emission = Vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);

	loadedScene->materials.push_back(material);
}

void SceneLoader::LoadBones(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap) {
	std::vector<Bone> bones;

	for (int32_t i = 0; i < animation->mNumChannels; i++) {
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			boneInfoMap[boneName].id = boneInfoMap.size();
		}

		Bone bone = Bone(boneName, boneInfoMap[boneName].id);

		for (int32_t positionIndex = 0; positionIndex < channel->mNumPositionKeys; positionIndex++) {
			KeyPosition data;
			data.position = AssimpGLMHelpers::GetGLMVec(channel->mPositionKeys[positionIndex].mValue);
			data.timeStamp = channel->mPositionKeys[positionIndex].mTime;
			bone.positions.push_back(data);
		}

		for (int32_t rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; rotationIndex++) {
			KeyRotation data;
			data.orientation = AssimpGLMHelpers::GetGLMQuat(channel->mRotationKeys[rotationIndex].mValue);
			data.timeStamp = channel->mRotationKeys[rotationIndex].mTime;
			bone.rotations.push_back(data);
		}

		for (int32_t keyIndex = 0; keyIndex < channel->mNumScalingKeys; keyIndex++) {
			KeyScale data;
			data.scale = AssimpGLMHelpers::GetGLMVec(channel->mScalingKeys[keyIndex].mValue);
			data.timeStamp = channel->mScalingKeys[keyIndex].mTime;
			bone.scales.push_back(data);
		}

		bones.push_back(bone);
	}
}

void SceneLoader::SetVertexBoneData(Vertex& vertex, int32_t boneID, Float weight) {
	for (uint32_t i = 0; i < MaxBonesPerVertex; i++) {
		if (vertex.boneIDs[i] < 0) {
			vertex.boneWeights[i] = weight;
			vertex.boneIDs[i] = boneID;
			break;
		}
	}
}

void SceneLoader::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
	if (!mesh->HasBones()) return;
	std::map<std::string, BoneInfo> boneInfoMap;
	int32_t boneCounter = 0;

	for (int32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int32_t boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCounter;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCounter;
			boneCounter++;
		}
		else {
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);

		aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
		int32_t numWeights = mesh->mBones[boneIndex]->mNumWeights;
		for (int32_t weightIndex = 0; weightIndex < numWeights; weightIndex++) {
			int32_t vertexId = weights[weightIndex].mVertexId;
			Float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}