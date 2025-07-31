#include "pch.h"
#include "ModelLoader.h"
#include "TextureLoader.h"
#include "Log.h"
#include "Globals.h"

void ModelLoader::LoadModel(std::shared_ptr<Scene> currentScene, const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = Globals::GetAssetsPath().string() + filePath.string();
	Log::Message("Loading model from file: %s", fullPath.string().c_str());
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fullPath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
		Log::Error("ASSIMP: %s", importer.GetErrorString());
		return;
	}

	std::map<std::string, BoneInfo> boneInfoMap;
	SceneObject* rootObject = ProcessAssimpNode(currentScene, fullPath, scene, scene->mRootNode, boneInfoMap);

	std::vector<Animation*> animations;
	for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
		std::vector<Bone> bones = ProcessAssimpAnimation(scene->mAnimations[i], boneInfoMap);
		animations.push_back(new Animation(
			(float)scene->mAnimations[i]->mDuration,
			(int32_t)scene->mAnimations[i]->mTicksPerSecond,
			bones, boneInfoMap, rootObject)
		);
	}
	if (animations.size() > 0) {
		glm::mat4 globalInverseTransform = glm::inverse(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));
		currentScene->CreateComponent<MeshAnimatorComponent>(rootObject, animations, globalInverseTransform);
	}

	importer.FreeScene();
}

Mesh ModelLoader::LoadMesh(const std::filesystem::path& filePath) {
	Log::Message("Loading mesh from file: %s", filePath.string().c_str());
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
		Log::Error("ASSIMP: %s", importer.GetErrorString());
		return {};
	}

	std::vector<Vertex> vertices;
	std::vector<int32_t> indices;
	for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];
		int32_t indicesOffset = vertices.size();

		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			vertices.push_back(Vertex(
				GetGLMVec(mesh->mVertices[i]),
				mesh->HasNormals() ? GetGLMVec(mesh->mNormals[i]) : glm::vec3(0.0f),
				mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f)
			));
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			bool validFace = true;
			for (uint32_t j = 0; j < face.mNumIndices; j++) {
				if (face.mIndices[j] >= vertices.size()) {
					validFace = false;
					break;
				}
			}
			if (!validFace) continue;
			for (uint32_t j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j] + indicesOffset);
			}
		}
	}

	importer.FreeScene();

	return Mesh(vertices, indices);
}

SceneObject* ModelLoader::ProcessAssimpNode(std::shared_ptr<Scene> currentScene, const std::filesystem::path& filePath, const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap) {
	Transform transform = Transform(ConvertMatrixToGLMFormat(node->mTransformation));
	SceneObject* nodeObject = currentScene->CreateObject(node->mName.data, nullptr, transform);

	if (node->mNumMeshes == 1) {
		aiMesh* aiMesh = scene->mMeshes[node->mMeshes[0]];
		Mesh mesh = ProcessAssimpMesh(aiMesh, boneInfoMap);
		MeshHandle meshHandle(mesh);
		currentScene->CreateComponent<MeshComponent>(nodeObject, meshHandle);
		std::shared_ptr<Material> material = nullptr;
		if (aiMesh->mMaterialIndex >= 0) {
			material = ProcessAssimpMaterial(filePath, scene->mMaterials[aiMesh->mMaterialIndex]);
		}
		else {
			material = std::make_shared<Material>("Default Material");
		}
		currentScene->CreateComponent<MaterialComponent>(nodeObject, material);
		if (material->m_emissionColor != glm::vec3(0) && material->m_emissionStrength != 0.0f) {
			currentScene->CreateComponent<DiffuseAreaLightComponent>(nodeObject, meshHandle, mesh, material->m_emissionColor, material->m_emissionStrength);
		}
	}
	else {
		for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
			SceneObject* child = currentScene->CreateObject("mesh holder", nodeObject);
			aiMesh* aiMesh = scene->mMeshes[node->mMeshes[meshIndex]];
			Mesh mesh = ProcessAssimpMesh(aiMesh, boneInfoMap);
			MeshHandle meshHandle(mesh);
			currentScene->CreateComponent<MeshComponent>(child, meshHandle);
			std::shared_ptr<Material> material = nullptr;
			if (aiMesh->mMaterialIndex >= 0) {
				material = ProcessAssimpMaterial(filePath, scene->mMaterials[aiMesh->mMaterialIndex]);
			}
			else {
				material = std::make_shared<Material>("Default Material");
			}
			currentScene->CreateComponent<MaterialComponent>(nodeObject, material);
			currentScene->SetObjectParent(child, nodeObject);
		}
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		SceneObject* child = ProcessAssimpNode(currentScene, filePath, scene, node->mChildren[i], boneInfoMap);
		currentScene->SetObjectParent(child, nodeObject);
	}

	return nodeObject;
}

std::vector<Bone> ModelLoader::ProcessAssimpAnimation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap) {
	std::vector<Bone> bones;

	for (uint32_t i = 0; i < animation->mNumChannels; i++) {
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			int32_t boneID = (int32_t)boneInfoMap.size();
			boneInfoMap[boneName] = { boneID, glm::mat4(1.0f) };
		}

		Bone bone = Bone(boneName, boneInfoMap[boneName].id);

		for (uint32_t positionIndex = 0; positionIndex < channel->mNumPositionKeys; positionIndex++) {
			KeyPosition data;
			data.position = GetGLMVec(channel->mPositionKeys[positionIndex].mValue);
			data.timeStamp = (float)channel->mPositionKeys[positionIndex].mTime;
			bone.positions.push_back(data);
		}

		for (uint32_t rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; rotationIndex++) {
			KeyRotation data;
			data.orientation = GetGLMQuat(channel->mRotationKeys[rotationIndex].mValue);
			data.timeStamp = (float)channel->mRotationKeys[rotationIndex].mTime;
			bone.rotations.push_back(data);
		}

		for (uint32_t scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; scaleIndex++) {
			KeyScale data;
			data.scale = GetGLMVec(channel->mScalingKeys[scaleIndex].mValue);
			data.timeStamp = (float)channel->mScalingKeys[scaleIndex].mTime;
			bone.scales.push_back(data);
		}

		bones.push_back(bone);
	}

	return bones;
}

Mesh ModelLoader::ProcessAssimpMesh(const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap) {
	std::vector<Vertex> vertices;
	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
		vertices.push_back(Vertex(
			GetGLMVec(mesh->mVertices[i]),
			mesh->HasNormals() ? GetGLMVec(mesh->mNormals[i]) : glm::vec3(0.0f),
			mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f)
		));
	}

	std::vector<int32_t> indices;
	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		bool validFace = true;
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			if (face.mIndices[j] >= vertices.size()) {
				validFace = false;
				break;
			}
		}
		if (!validFace) continue;
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int32_t boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			boneID = (int32_t)boneInfoMap.size();
			boneInfoMap[boneName] = { boneID, ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix) };
		}
		else {
			boneID = boneInfoMap[boneName].id;
		}
		assert(boneID != -1);

		aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
		for (uint32_t weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; weightIndex++) {
			assert(weights[weightIndex].mVertexId <= vertices.size());
			vertices[weights[weightIndex].mVertexId].AddWeight(boneID, weights[weightIndex].mWeight);
		}
	}

	return Mesh(vertices, indices);
}

std::shared_ptr<Material> ModelLoader::ProcessAssimpMaterial(const std::filesystem::path& filePath, const aiMaterial* aiMaterial) {
	std::string materialName = aiMaterial->GetName().C_Str();

	aiColor4D color, colorEmissive;
	float emissionInt = 0.0f, metallic = 0.0f, opacity = 0.0f, eta = 0.0f, roughness = 1.0f;
	aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
	aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &colorEmissive);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_EMISSIVE_INTENSITY, &emissionInt);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_METALLIC_FACTOR, &metallic);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &roughness);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_OPACITY, &opacity);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_REFRACTI, &eta);

	glm::vec3 glmEmissionColor = glm::vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);
	float emissionNormaizer = (float)MaxComponent(glmEmissionColor);
	glmEmissionColor /= emissionNormaizer;
	emissionInt = emissionNormaizer * (emissionInt ? emissionInt : 1.0f);

	std::vector<Texture> diffuseMaps = ProcessAssimpMaterialTextures(filePath, aiMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
	std::vector<Texture> specularMaps = ProcessAssimpMaterialTextures(filePath, aiMaterial, aiTextureType_SPECULAR, "texture_specular");

	if (isnan(glmEmissionColor.r) || isnan(glmEmissionColor.y) || isnan(glmEmissionColor.z)) {
		glmEmissionColor = glm::vec3(0);
	}
	if (isnan(emissionInt)) {
		emissionInt = 0.0f;
	}
	if (isnan(roughness)) {
		roughness = 1.0f;
	}
	if (isnan(metallic)) {
		metallic = 0.0f;
	}
	if (isnan(opacity)) {
		opacity = 0.0f;
	}
	if (isnan(eta)) {
		eta = 1.0f;
	}

	glm::vec3 albedo = glm::vec3(color.r, color.g, color.b);
	std::shared_ptr<Material> material = std::make_shared<Material>(materialName, albedo, glmEmissionColor, emissionInt, roughness, metallic, 1.0f - opacity, eta);
	if (diffuseMaps.size() > 0) {
		material->m_albedoTexture = diffuseMaps[0];
	}

	return material;
}

std::vector<Texture> ModelLoader::ProcessAssimpMaterialTextures(const std::filesystem::path& filePath, const aiMaterial* material, aiTextureType type, const std::string&) {
	std::vector<Texture> textures;
	for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		std::filesystem::path texturePath = filePath.parent_path().string() + "/" + str.C_Str();
		textures.push_back(TextureLoader::LoadTextureRGB(texturePath));
	}
	return textures;
}


glm::mat4 ModelLoader::ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

glm::vec2 ModelLoader::GetGLMVec(const aiVector2D& vec) {
	return glm::vec2(vec.x, vec.y);
}

glm::vec3 ModelLoader::GetGLMVec(const aiVector3D& vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat ModelLoader::GetGLMQuat(const aiQuaternion& pOrientation) {
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}
