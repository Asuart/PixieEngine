#include "pch.h"
#include "ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::filesystem::path ResourceManager::m_currentFilePath = "";
std::map<std::filesystem::path, Scene*> ResourceManager::m_scenes = {};
std::map<std::filesystem::path, SceneObject*> ResourceManager::m_Models = {};
std::map<std::filesystem::path, Texture<Vec3>*> ResourceManager::m_RGBTextures = {};
std::map<std::string, Material*> ResourceManager::m_materials = {};
std::vector<Mesh*> ResourceManager::m_meshes = {};
Material* ResourceManager::m_defaultMaterial = nullptr;

void ResourceManager::Initialize() {
	m_defaultMaterial = new Material("Default Material");
}

Scene* ResourceManager::LoadScene(std::filesystem::path filePath) {
	m_currentFilePath = filePath;
	if (!CheckFileExtensionSupport(filePath, ResourceType::Scene)) {
		return nullptr;
	}

	if (m_scenes.find(filePath) != m_scenes.end()) {
		return m_scenes[filePath];
	}

	Scene* scene = nullptr;
	if (filePath.extension() == ".scene") {
		Scene* scene = LoadPixieEngineScene(filePath);
	} 
	else if (filePath.extension() == ".pbrt") {
		Scene* scene = LoadPBRTScene(filePath);
	}
	else {
		return nullptr;
	}

	m_scenes[filePath] = scene;

	return scene;
}

bool ResourceManager::SaveScene(std::filesystem::path filePath, Scene* scene) {
	m_currentFilePath = filePath;
	std::cout << "Error: Scene saving is not implemented.\n";
	return false;
}

SceneObject* ResourceManager::LoadModel(std::filesystem::path filePath) {
	m_currentFilePath = filePath;
	std::cout << "Loading model from file: " << filePath << "\n";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
		return nullptr;
	}

	std::map<std::string, BoneInfo> boneInfoMap;
	SceneObject* rootObject = ProcessAssimpNode(scene, scene->mRootNode, boneInfoMap);

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
		Mat4 globalInverseTransform = glm::inverse(AssimpGLMHelpers::ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));
		MeshAnimatorComponent* animator = new MeshAnimatorComponent(animations, rootObject, globalInverseTransform);
		rootObject->AddComponent(animator);
	}

	return rootObject;
}

Texture<Vec3>* ResourceManager::LoadRGBTexture(std::filesystem::path filePath) {
	std::cout << "  Loading texture: " << filePath << "\n";
	if (m_RGBTextures.find(filePath) != m_RGBTextures.end()) {
		std::cout << "  Log: Texture already loaded.\n";
		return m_RGBTextures[filePath];
	}
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		std::cout << "  Error: Failed to load texture.\n";
		return nullptr;
	}
	if (nrChannels != 3 && nrChannels != 4) {
		std::cout << "  Error: Only 3 and 4 channel textures are suported.\n";
		stbi_image_free(data);
		return nullptr;
	}
	Texture<Vec3>* texture = new Texture<Vec3>(width, height, data, nrChannels);
	stbi_image_free(data);
	return texture;
}

Material* ResourceManager::GetDefaultMaterial() {
	return m_defaultMaterial;
}

bool ResourceManager::CheckFileExtensionSupport(std::filesystem::path filePath, ResourceType type) {
	if (!filePath.has_extension()) {
		std::cout << "Provided path doesn't have an extension: " << filePath << "\n";
		return false;
	}

	const std::vector<std::string> sceneExtensions = { ".scene", ".pbrt" };
	const std::vector<std::string> modelExtensions = { ".dae", ".obj", ".fbx", ".glb", ".ply" };
	const std::vector<std::string> RGBTextureExtensions = { ".png" };

	std::string extension = filePath.extension().string();
	switch (type) {
	case ResourceType::Scene:
		if (std::find(sceneExtensions.begin(), sceneExtensions.end(), extension) == sceneExtensions.end()) {
			std::cout << "Unsupported scene format: " << filePath << "\n";
			return false;
		}
		break;
	case ResourceType::Model:
		if (std::find(modelExtensions.begin(), modelExtensions.end(), extension) == modelExtensions.end()) {
			std::cout << "Unsupported model format: " << filePath << "\n";
			return false;
		}
		break;
	case ResourceType::RGBTexture:
		if (std::find(RGBTextureExtensions.begin(), RGBTextureExtensions.end(), extension) == RGBTextureExtensions.end()) {
			std::cout << "Unsupported texture format: " << filePath << "\n";
			return false;
		}
		break;
	default:
		std::cout << "Unexpected resource type provided.\n";
		return false;
		break;
	}

	return true;
}

Scene* ResourceManager::LoadPixieEngineScene(std::filesystem::path path) {
	std::cout << "Error: Pixie Engine scene loading is not implemented.\n";
	return nullptr;
}

Scene* ResourceManager::LoadPBRTScene(std::filesystem::path filePath) {
	std::cout << "Error: PBRT scene loading is not implemented.\n";
	return nullptr;
}

SceneObject* ResourceManager::ProcessAssimpNode(const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap) {
	std::cout << "  Node: " << node->mName.C_Str() << " (children: " << node->mNumChildren << ", meshes: " << node->mNumMeshes << ")\n";
	
	SceneObject* nodeObject = new SceneObject(node->mName.data, Transform(AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation)));
	
	for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];
		SceneObject* meshObject = ProcessAssimpMesh(scene, mesh, boneInfoMap);
		nodeObject->AddChild(meshObject);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		SceneObject* child = ProcessAssimpNode(scene, node->mChildren[i], boneInfoMap);
		nodeObject->AddChild(child);
	}

	return nodeObject;
}

std::vector<Bone> ResourceManager::ProcessAssimpAnimation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap) {
	std::vector<Bone> bones;

	for (uint32_t i = 0; i < animation->mNumChannels; i++) {
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			int32_t boneID = (int32_t)boneInfoMap.size();
			boneInfoMap[boneName] = { boneID, Mat4(1.0f) };
		}

		Bone bone = Bone(boneName, boneInfoMap[boneName].id);

		for (uint32_t positionIndex = 0; positionIndex < channel->mNumPositionKeys; positionIndex++) {
			KeyPosition data;
			data.position = AssimpGLMHelpers::GetGLMVec(channel->mPositionKeys[positionIndex].mValue);
			data.timeStamp = (Float)channel->mPositionKeys[positionIndex].mTime;
			bone.positions.push_back(data);
		}

		for (uint32_t rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; rotationIndex++) {
			KeyRotation data;
			data.orientation = AssimpGLMHelpers::GetGLMQuat(channel->mRotationKeys[rotationIndex].mValue);
			data.timeStamp = (Float)channel->mRotationKeys[rotationIndex].mTime;
			bone.rotations.push_back(data);
		}

		for (uint32_t scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; scaleIndex++) {
			KeyScale data;
			data.scale = AssimpGLMHelpers::GetGLMVec(channel->mScalingKeys[scaleIndex].mValue);
			data.timeStamp = (Float)channel->mScalingKeys[scaleIndex].mTime;
			bone.scales.push_back(data);
		}

		bones.push_back(bone);
	}

	return bones;
}

SceneObject* ResourceManager::ProcessAssimpMesh(const aiScene* scene, const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap) {
	SceneObject* object = new SceneObject(mesh->mName.C_Str());

	std::vector<Vertex> vertices;
	for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
		vertices.push_back(Vertex(
			AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]),
			mesh->HasNormals() ? AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]) : Vec3(0.0f),
			mesh->HasTextureCoords(0) ? Vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : Vec2(0.0f)
		));
	}

	std::vector<int32_t> indices;
	for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int32_t boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			boneID = (int32_t)boneInfoMap.size();
			boneInfoMap[boneName] = { boneID, AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix) };
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

	MeshComponent* meshComponent = new MeshComponent(new Mesh(vertices, indices), object);
	object->AddComponent(meshComponent);

	Material* material = m_defaultMaterial;
	if (mesh->mMaterialIndex >= 0) {
		material = ProcessAssimpMaterial(scene->mMaterials[mesh->mMaterialIndex]);
	}
	MaterialComponent* materialComponent = new MaterialComponent(material, object);
	object->AddComponent(materialComponent);

	return object;
}

Material* ResourceManager::ProcessAssimpMaterial(const aiMaterial* aiMaterial) {
	std::string materialName = aiMaterial->GetName().C_Str();
	std::cout << "  Material: " << materialName << "\n";

	if (auto it = m_materials.find(materialName); it != m_materials.end()) {
		return it->second;
	}

	aiColor4D color, colorEmissive;
	float emissionInt = 0.0f, roughness = 0.0f, opacity = 0.0f, eta = 0.0f;
	aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
	aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &colorEmissive);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_EMISSIVE_INTENSITY, &emissionInt);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &roughness);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_OPACITY, &opacity);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_REFRACTI, &eta);

	Vec3 glmEmissionColor = Vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);
	float emissionNormaizer = (float)MaxComponent(glmEmissionColor);
	glmEmissionColor /= emissionNormaizer;
	emissionInt = emissionNormaizer * (emissionInt ? emissionInt : 1.0f);

	std::vector<Texture<Vec3>*> diffuseMaps = ProcessAssimpMaterialTextures(aiMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
	std::vector<Texture<Vec3>*> specularMaps = ProcessAssimpMaterialTextures(aiMaterial, aiTextureType_SPECULAR, "texture_specular");

	Material* material = new Material(materialName);
	material->m_albedo = Vec3(color.r, color.g, color.b);
	if (diffuseMaps.size() > 0) {
		material->m_albedoTexture = diffuseMaps[0];
	}
	material->m_roughness = roughness;
	if (specularMaps.size() > 0) {

	}
	material->m_emissionColor = glmEmissionColor;
	material->m_emissionStrength = emissionInt;
	material->m_transparency = 1.0f - opacity;
	material->m_refraction = eta;

	m_materials[materialName] = material;

	return material;
}

std::vector<Texture<Vec3>*> ResourceManager::ProcessAssimpMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name) {
	std::vector<Texture<Vec3>*> textures;
	for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		std::filesystem::path texturePath = m_currentFilePath.parent_path().string() + "/" + str.C_Str();
		textures.push_back(LoadRGBTexture(texturePath));
	}
	return textures;
}
