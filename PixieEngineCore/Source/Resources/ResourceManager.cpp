#include "pch.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "GlobalRenderer.h"
#include "TextureGenerator.h"
#include "MeshGenerator.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::filesystem::path ResourceManager::m_applicationPath = "";
std::filesystem::path ResourceManager::m_currentFilePath = "";
std::filesystem::path ResourceManager::m_assetsPath = "../Assets/Scenes";
std::map<std::filesystem::path, SceneObject*> ResourceManager::m_models = {};
std::map<std::filesystem::path, Texture> ResourceManager::m_textures = {};
std::vector<Material> ResourceManager::m_materials = {};
std::vector<Mesh*> ResourceManager::m_meshes = {};
std::map<char, FontCharacter> ResourceManager::m_characters = {};
uint32_t ResourceManager::m_defaultFontSize = 64;

static const std::string c_deafultMaterial = "Default Material";
static const int32_t c_maxMaterials = 512;

inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

inline void trim(std::string& s) {
	rtrim(s);
	ltrim(s);
}

Mat4 AssimpGLMHelpers::ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
	Mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

Vec2 AssimpGLMHelpers::GetGLMVec(const aiVector2D& vec) {
	return Vec2(vec.x, vec.y);
}

Vec3 AssimpGLMHelpers::GetGLMVec(const aiVector3D& vec) {
	return Vec3(vec.x, vec.y, vec.z);
}

Quaternion AssimpGLMHelpers::GetGLMQuat(const aiQuaternion& pOrientation) {
	return Quaternion(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

void ResourceManager::SetApplicationPath(const std::string& path) {
	m_applicationPath = path;
}

std::filesystem::path ResourceManager::GetApplicationPath() {
	return m_applicationPath;
}

std::filesystem::path ResourceManager::GetApplicationDirectory() {
	return m_applicationPath.parent_path();
}

std::filesystem::path ResourceManager::GetAssetsPath() {
	return m_assetsPath;
}

void ResourceManager::SetAssetsPath(const std::filesystem::path& path) {
	m_assetsPath = path;
}

void ResourceManager::Initialize() {
	m_meshes.push_back(MeshGenerator::Quad({ 0, 0 }, { 1, 1 }));
	m_meshes.push_back(MeshGenerator::Cube(Vec3(1)));
	m_meshes.push_back(MeshGenerator::SphereFromOctahedron(1.0f, 6));

	m_materials.reserve(c_maxMaterials);
	AddMaterial(Material(c_deafultMaterial));

	LoadDefaultFont();

	m_textures.insert({"brdfLUT", Texture({512, 512}, GL_RG16F, TextureWrap::ClampToEdge, TextureWrap::ClampToEdge, TextureFiltering::Linear, TextureFiltering::Linear) });
	GlobalRenderer::DrawBRDFLookUpTexture({ 512, 512 }, m_textures["brdfLUT"].GetHandle());
}

std::shared_ptr<Scene> ResourceManager::LoadScene(const std::filesystem::path& filePath) {
	m_currentFilePath = filePath;
	std::shared_ptr<Scene> scene = nullptr;
	if (!CheckFileExtensionSupport(filePath, ResourceType::Scene)) {
		return scene;
	}

	if (filePath.extension() == ".scene") {
		scene = LoadPixieEngineScene(filePath);
	} 
	else if (filePath.extension() == ".pbrt") {
		scene = LoadPBRTScene(filePath);
	}

	return scene;
}

bool ResourceManager::SaveScene(const std::filesystem::path& filePath, Scene* scene) {
	m_currentFilePath = filePath;
	std::cout << "Error: Scene saving is not implemented.\n";
	return false;
}

SceneObject* ResourceManager::LoadModel(const std::filesystem::path& filePath) {
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
		SceneManager::CreateComponent<MeshAnimatorComponent>(rootObject, animations, globalInverseTransform);
	}

	importer.FreeScene();

	return rootObject;
}

Texture ResourceManager::LoadTexture(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	std::cout << "  Loading texture: " << filePath << "\n";
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 0);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 0);
		if (!data) {
			std::cout << "  Error: Failed to load texture.\n";
			return Texture();
		}
	}
	switch (nrChannels) {
	case 1: {
		Texture texture({ width, height }, GL_R32F, GL_RED, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		return texture;
	}
	case 3: {
		Texture texture({ width, height }, GL_RGB32F, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		return texture;
	}
	case 4: {
		Texture texture({ width, height }, GL_RGBA32F, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		return texture;
	}
	default:
		std::cout << "  Error: Only 1, 3 and 4 channel textures are suported.\n";
		stbi_image_free(data);
		return Texture();
	}
}

Texture ResourceManager::LoadTextureFloat(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	std::cout << "  Loading texture: " << filePath << "\n";
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 1);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 1);
		if (!data) {
			std::cout << "  Error: Failed to load texture.\n";
			return Texture();
		}
	}
	Texture texture({ width, height }, GL_R32F, GL_RED, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	return texture;
}

Texture ResourceManager::LoadTextureRGB(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	std::cout << "  Loading texture: " << filePath << "\n";
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 3);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 3);
		if (!data) {
			std::cout << "  Error: Failed to load texture.\n";
			return Texture();
		}
	}
	Texture texture({ width, height }, GL_RGB32F, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	return texture;
}

Texture ResourceManager::LoadTextureRGBA(const std::filesystem::path& filePath) {
	std::filesystem::path fullPath = GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
	std::cout << "  Loading texture: " << filePath << "\n";
	int32_t width, height, nrChannels;
	uint8_t* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 4);
	if (!data) {
		data = stbi_load(fullPath.string().c_str(), &width, &height, &nrChannels, 4);
		if (!data) {
			std::cout << "  Error: Failed to load texture.\n";
			return Texture();
		}
	}
	Texture texture({ width, height }, GL_RGBA32F, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	return texture;
}

Buffer2DTexture<Float> ResourceManager::LoadBuffer2DTextureFloat(const std::filesystem::path& filePath) {
	std::cout << "  Loading texture: " << filePath << "\n";
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 1);
	if (!data) {
		std::filesystem::path fullPath = GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 1);
		if (!data) {
			std::cout << "  Error: Failed to load texture.\n";
			return Buffer2DTexture<Float>();
		}
	}
	Buffer2D<Float> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = data[i];
	}
	stbi_image_free(data);
	return Buffer2DTexture<Float>(buffer);
}

Buffer2DTexture<Vec3> ResourceManager::LoadBuffer2DTextureRGB(const std::filesystem::path& filePath) {
	std::cout << "  Loading texture: " << filePath << "\n";
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 3);
	if (!data) {
		std::filesystem::path fullPath = GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 3);
		if (!data) {
			std::cout << "  Error: Failed to load texture.\n";
			return Buffer2DTexture<Vec3>();
		}
	}
	Buffer2D<Vec3> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = Vec3(data[i * 3 + 0], data[i * 3 + 1], data[i * 3 + 2]);
	}
	stbi_image_free(data);
	return Buffer2DTexture<Vec3>(buffer);
}

Buffer2DTexture<Vec4> ResourceManager::LoadBuffer2DTextureRGBA(const std::filesystem::path& filePath) {
	std::cout << "  Loading texture: " << filePath << "\n";
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 4);
	if (!data) {
		std::filesystem::path fullPath = GetApplicationDirectory().string() + std::string("/Resources/Textures/") + filePath.string();
		data = stbi_loadf(fullPath.string().c_str(), &width, &height, &nrChannels, 4);
		if (!data) {
			std::cout << "  Error: Failed to load texture.\n";
			return Buffer2DTexture<Vec4>();
		}
	}
	Buffer2D<Vec4> buffer({ width, height });
	for (int32_t i = 0; i < width * height; i++) {
		buffer.m_data[i] = Vec4(data[i * 4 + 0], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
	}
	stbi_image_free(data);
	return Buffer2DTexture<Vec4>(buffer);
}

HDRISkybox ResourceManager::LoadSkybox(const std::filesystem::path& path) {
	Buffer2DTexture<Vec3> sphericalMap = LoadBuffer2DTextureRGB(GetApplicationDirectory().string() + std::string("/Resources/Skymaps/") + path.string());
	return TextureGenerator::Skybox(sphericalMap, { 512, 512 }, { 32, 32 }, { 128, 128 });
}

Material* ResourceManager::GetDefaultMaterial() {
	return &m_materials[0];
}

Material* ResourceManager::GetMaterial(uint32_t index) {
	return &m_materials[index];
}

int32_t ResourceManager::GetMaterialIndex(const Material* material) {
	for (int32_t i = 0; i < m_materials.size(); i++) {
		if (m_materials[i].m_name == material->m_name) {
			return i;
		}
	}
	return 0;
}

int32_t ResourceManager::GetMaterialIndex(const std::string& name) {
	for (int32_t i = 0; i < m_materials.size(); i++) {
		if (m_materials[i].m_name == name) {
			return i;
		}
	}
	return 0;
}

std::vector<Material>& ResourceManager::GetMaterials() {
	return m_materials;
}

bool ResourceManager::IsValidScenePath(const std::filesystem::path& filePath) {
	if (!filePath.has_extension()) {
		std::cout << "Provided path doesn't have an extension: " << filePath << "\n";
		return false;
	}
	std::string extension = filePath.extension().string();
	const std::set<std::string> sceneExtensions = { ".scene", ".pbrt" };
	if (std::find(sceneExtensions.begin(), sceneExtensions.end(), extension) == sceneExtensions.end()) {
		std::cout << "Unsupported scene format: " << filePath << "\n";
		return false;
	}
	return true;
}

bool ResourceManager::IsValidModelPath(const std::filesystem::path& filePath) {
	if (!filePath.has_extension()) {
		std::cout << "Provided path doesn't have an extension: " << filePath << "\n";
		return false;
	}
	std::string extension = filePath.extension().string();
	const std::set<std::string> modelExtensions = { ".dae", ".obj", ".fbx", ".glb", ".gltf", ".ply", ".3ds"};
	if (std::find(modelExtensions.begin(), modelExtensions.end(), extension) == modelExtensions.end()) {
		std::cout << "Unsupported model format: " << filePath << "\n";
		return false;
	}
	return true;
}

bool ResourceManager::IsValidTexturePath(const std::filesystem::path& filePath) {
	if (!filePath.has_extension()) {
		std::cout << "Provided path doesn't have an extension: " << filePath << "\n";
		return false;
	}
	std::string extension = filePath.extension().string();
	const std::set<std::string> RGBTextureExtensions = { ".png" };
	if (std::find(RGBTextureExtensions.begin(), RGBTextureExtensions.end(), extension) == RGBTextureExtensions.end()) {
		std::cout << "Unsupported texture format: " << filePath << "\n";
		return false;
	}
	return true;
}

Mesh* ResourceManager::GetQuadMesh() {
	return m_meshes[0];
}

Mesh* ResourceManager::GetCubeMesh() {
	return m_meshes[1];
}

Mesh* ResourceManager::GetSphereMesh() {
	return m_meshes[2];
}

bool ResourceManager::CheckFileExtensionSupport(const std::filesystem::path& filePath, ResourceType type) {
	switch (type) {
	case ResourceType::Scene:
		return IsValidScenePath(filePath);
	case ResourceType::Model:
		return IsValidModelPath(filePath);
	case ResourceType::RGBTexture:
		return IsValidModelPath(filePath);
	default:
		std::cout << "Unexpected resource type provided.\n";
		return false;
	}
}

std::shared_ptr<Scene> ResourceManager::LoadPixieEngineScene(const std::filesystem::path& path) {
	std::cout << "Error: Pixie Engine scene loading is not implemented.\n";
	return nullptr;
}

std::shared_ptr<Scene> ResourceManager::LoadPBRTScene(const std::filesystem::path& filePath) {
	std::ifstream reader;
	reader.open(filePath);
	std::string line;

	std::shared_ptr<Scene> scene = SceneManager::CreateScene(filePath.filename().string());

	const int32_t maxDepth = 6;
	int32_t depth = 0;
	std::vector<SceneObject*> objects;
	objects.resize(maxDepth);
	objects[0] = scene->GetRootObject();
	while (std::getline(reader, line)) {
		trim(line);
		if (line == "") {
			continue;
		}
		if (line.find("#") != -1) {
			continue;
		}
		if (line.find("[") != -1) {
			if (line.find("]") == -1) {
				std::string l;
				while (std::getline(reader, l)) {
					trim(l);
					line += " " + l;
					if (l.find("]") != -1) {
						break;
					}
				}
			}
		}
		std::vector<std::string> tokens = SplitPBRTFileLine(line);
		if (tokens.size() == 0) {
			continue;
		}
		if (tokens[0] == "AttributeBegin" || tokens[0] == "ObjectBegin") {
			depth++;
			if (depth == maxDepth) {
				std::cout << "Error parsing PBRT scene. Max node depth exceeded.\n";
				return scene;
			}
			objects[depth] = SceneManager::CreateObject("node", objects[depth - 1]);
			continue;
		}
		else if (tokens[0] == "AttributeEnd" || tokens[0] == "ObjectEnd") {
			depth--;
			if (depth == -1) {
				std::cout << "Error parsing PBRT scene. There are more AttributeEnd lines than AttributeBegin.\n";
				return scene;
			}
			continue;
		}
		else if (tokens[0] == "Transform") {
			if (tokens.size() != 19) {
				std::cout << "Unexpected tokens amount parsing Transform: " << tokens.size() << "\n";
				break;
			}
			if (tokens[1] != "[" || tokens[18] != "]") {
				std::cout << "Error parsing Transform. Values are not inside []\n";
				break;
			}
			Mat4 transform = Mat4(1.0f);
			for (int32_t i = 0; i < 16; i++) {
				transform[i / 4][i % 4] = StringToFloat(tokens[i + 2]);
			}
			objects[depth]->GetTransform().Set(transform);
			continue;
		}
		else if (tokens[0] == "NamedMaterial") {
			if (tokens.size() != 2) {
				std::cout << "Unexpected tokens amount parsing NamedMaterial: " << tokens.size() << "\n";
				break;
			}
			Material* material = FindMaterial(tokens[1]);
			if (material == nullptr) {
				material = AddMaterial(Material(tokens[1]));
			}
			if (MaterialComponent* materialComponent = objects[depth]->GetComponent<MaterialComponent>()) {
				materialComponent->SetMaterial(material);
			}
			else {
				materialComponent = SceneManager::CreateComponent<MaterialComponent>(objects[depth], material);
			}
			continue;
		}
		else if (tokens[0] == "Shape") {
			if (!objects[depth]->GetComponent<MaterialComponent>()) {
				SceneManager::CreateComponent< MaterialComponent>(objects[depth], GetDefaultMaterial());
			}
			if (tokens[1] == "trianglemesh") {
				if (tokens.size() != 2) {
					std::cout << "Unexpected tokens amount parsing Shape trianglemesh: " << tokens.size() << "\n";
					break;
				}
				SceneManager::CreateComponent< MeshComponent>(objects[depth], new Mesh({}, {}));
				continue;
			}
			else if (tokens[1] == "plymesh") {
				if (tokens.size() != 4) {
					std::cout << "Unexpected tokens amount parsing Shape plymesh: " << tokens.size() << "\n";
					break;
				}
				if (tokens[2] != "string filename") {
					std::cout << "Unexpected token parsing Shape plymesh: " << tokens[2] << "\n";
					break;
				}
				std::string modelPath = tokens[3];
				SceneObject* model = LoadModel(filePath.parent_path().string() + "/" + modelPath);
				if (!model) {
					break;
				}
				SceneManager::SetObjectParent(model, objects[depth]);
				continue;
			}
			std::cout << "Unexpected tokens amount parsing Shape - unexpected shape: " << tokens[1] << "\n";
			break;
		}
		else if (tokens[0] == "point2 uv") {
			MeshComponent* meshComponent = objects[depth]->GetComponent<MeshComponent>();
			if (tokens.size() < 5) {
				std::cout << "Error parsing point2 uv. Not enogh tokens.\n";
				break;
			}
			if (!meshComponent) {
				std::cout << "Error parsing PBRT scene. point2 uv are specidied for object without mesh.\n";
				break;
			}
			if (tokens[1] != "[" || tokens[tokens.size() - 1] != "]") {
				std::cout << "Error parsing point2 uv. Values are not inside []\n";
				break;
			}
			int32_t valuesCount = (int32_t)tokens.size() - 3;
			if (valuesCount % 2) {
				std::cout << "Error parsing point2 uv. Values are not aligned.\n";
				break;
			}
			int32_t verticesCount = valuesCount / 2;
			Mesh* mesh = meshComponent->GetMesh();
			if (mesh->m_vertices.size() < verticesCount) {
				mesh->m_vertices.resize(verticesCount);
			}
			for (int32_t i = 0, j = 0; i < valuesCount; i += 2, j++) {
				mesh->m_vertices[j].uv = Vec2(StringToFloat(tokens[i + 2]), StringToFloat(tokens[i + 3]));
			}
			meshComponent->UploadMesh();
			continue;
		}
		else if (tokens[0] == "normal N") {
			MeshComponent* meshComponent = objects[depth]->GetComponent<MeshComponent>();
			if (tokens.size() < 6) {
				std::cout << "Error parsing normal N. Not enogh tokens.\n";
				break;
			}
			if (!meshComponent) {
				std::cout << "Error parsing PBRT scene. normal N are specidied for object without mesh.\n";
				break;
			}
			if (tokens[1] != "[" || tokens[tokens.size() - 1] != "]") {
				std::cout << "Error parsing normal N. Values are not inside []\n";
				break;
			}
			int32_t valuesCount = (int32_t)tokens.size() - 3;
			if (valuesCount % 3) {
				std::cout << "Error parsing normal N. Values are not aligned.\n";
				break;
			}
			int32_t verticesCount = valuesCount / 3;
			Mesh* mesh = meshComponent->GetMesh();
			if (mesh->m_vertices.size() < verticesCount) {
				mesh->m_vertices.resize(verticesCount);
			}
			for (int32_t i = 0, j = 0; i < valuesCount; i += 3, j++) {
				mesh->m_vertices[j].normal = Vec3(StringToFloat(tokens[i + 2]), StringToFloat(tokens[i + 3]), StringToFloat(tokens[i + 4]));
			}
			meshComponent->UploadMesh();
			continue;
		}
		else if (tokens[0] == "point3 P") {
			MeshComponent* meshComponent = objects[depth]->GetComponent<MeshComponent>();
			if (tokens.size() < 6) {
				std::cout << "Error parsing point3 P. Not enogh tokens.\n";
				break;
			}
			if (!meshComponent) {
				std::cout << "Error parsing PBRT scene. point3 P are specidied for object without mesh.\n";
				break;
			}
			if (tokens[1] != "[" || tokens[tokens.size() - 1] != "]") {
				std::cout << "Error parsing point3 P. Values are not inside []\n";
				break;
			}
			int32_t valuesCount = (int32_t)tokens.size() - 3;
			if (valuesCount % 3) {
				std::cout << "Error parsing point3 P. Values are not aligned.\n";
				break;
			}
			int32_t verticesCount = valuesCount / 3;
			Mesh* mesh = meshComponent->GetMesh();
			if (mesh->m_vertices.size() < verticesCount) {
				mesh->m_vertices.resize(verticesCount);
			}
			for (int32_t i = 0, j = 0; i < valuesCount; i += 3, j++) {
				mesh->m_vertices[j].position = Vec3(StringToFloat(tokens[i + 2]), StringToFloat(tokens[i + 3]), StringToFloat(tokens[i + 4]));
			}
			meshComponent->UploadMesh();
			continue;
		}
		else if (tokens[0] == "integer indices") {
			MeshComponent* meshComponent = objects[depth]->GetComponent<MeshComponent>();
			if (tokens.size() < 6) {
				std::cout << "Error parsing integer indices. Not enogh tokens.\n";
				break;
			}
			if (!meshComponent) {
				std::cout << "Error parsing PBRT scene. integer indices are specidied for object without mesh.\n";
				break;
			}
			if (tokens[1] != "[" || tokens[tokens.size() - 1] != "]") {
				std::cout << "Error parsing integer indices. Values are not inside []\n";
				break;
			}
			int32_t valuesCount = (int32_t)tokens.size() - 3;
			if (valuesCount % 3) {
				std::cout << "Error parsing integer indices. Values are not aligned.\n";
				break;
			}
			Mesh* mesh = meshComponent->GetMesh();
			if (mesh->m_indices.size() < valuesCount) {
				mesh->m_indices.resize(valuesCount);
			}
			for (int32_t i = 0; i < valuesCount; i++) {
				mesh->m_indices[i] = std::stoi(tokens[i + 2]);
			}
			meshComponent->UploadMesh();
			continue;
		}
		else if (tokens[0] == "MediumInterface") {
			std::cout << "Warning: MediumInterface directive is ignored.\n";
			continue;
		}
		else if (tokens[0] == "texture displacement") {
			std::cout << "Warning: texture displacement directive is ignored.\n";
			continue;
		}
		else if (tokens[0] == "float edgelength") {
			std::cout << "Warning: float edgelength directive is ignored.\n";
			continue;
			}
		else if (tokens[0] == "Identity") {
			std::cout << "Warning: Identity directive is ignored.\n";
			continue;
		}
		else if (tokens[0] == "ObjectInstance") {
			std::cout << "Warning: ObjectInstance directive is ignored.\n";
			continue;
		}
		else if (tokens[0] == "Translate") {
			std::cout << "Warning: Translate directive is ignored.\n";
			continue;
		}
		std::cout << "Unexpected first token: " << tokens[0] << "\n";
		break;
	}
	reader.close();
	return scene;
}

SceneObject* ResourceManager::ProcessAssimpNode(const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap) {
	std::cout << "  Node: " << node->mName.C_Str() << " (children: " << node->mNumChildren << ", meshes: " << node->mNumMeshes << ")\n";
	
	Transform transform = Transform(AssimpGLMHelpers::ConvertMatrixToGLMFormat(node->mTransformation));
	SceneObject* nodeObject = SceneManager::CreateObject(node->mName.data, nullptr, transform);

	if (node->mNumMeshes == 1) {
		aiMesh* aiMesh = scene->mMeshes[node->mMeshes[0]];
		Mesh* mesh = ProcessAssimpMesh(aiMesh, boneInfoMap);
		SceneManager::CreateComponent<MeshComponent>(nodeObject, mesh);
		Material* material = GetDefaultMaterial();
		if (aiMesh->mMaterialIndex >= 0) {
			material = ProcessAssimpMaterial(scene->mMaterials[aiMesh->mMaterialIndex]);
		}
		SceneManager::CreateComponent<MaterialComponent>(nodeObject, material);
		if (material->GetEmission()) {
			SceneManager::CreateComponent<AreaLightComponent>(nodeObject, material->GetEmission().GetRGB(), 1.0f);
		}
	}
	else {
		for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
			SceneObject* child = SceneManager::CreateObject("mesh holder", nodeObject);
			aiMesh* aiMesh = scene->mMeshes[node->mMeshes[meshIndex]];
			Mesh* mesh = ProcessAssimpMesh(aiMesh, boneInfoMap);
			SceneManager::CreateComponent<MeshComponent>(child, mesh);
			Material* material = GetDefaultMaterial();
			if (aiMesh->mMaterialIndex >= 0) {
				material = ProcessAssimpMaterial(scene->mMaterials[aiMesh->mMaterialIndex]);
			}
			SceneManager::CreateComponent<MaterialComponent>(nodeObject, material);
			SceneManager::SetObjectParent(child, nodeObject);
		}
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		SceneObject* child = ProcessAssimpNode(scene, node->mChildren[i], boneInfoMap);
		SceneManager::SetObjectParent(child, nodeObject);
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

Mesh* ResourceManager::ProcessAssimpMesh(const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap) {
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

	return new Mesh(vertices, indices);
}

Material* ResourceManager::ProcessAssimpMaterial(const aiMaterial* aiMaterial) {
	std::string materialName = aiMaterial->GetName().C_Str();
	std::cout << "  Material: " << materialName << "\n";

	if (Material* mat = FindMaterial(materialName); mat != nullptr) {
		return mat;
	}

	aiColor4D color, colorEmissive;
	float emissionInt = 0.0f, metallic = 0.0f, opacity = 0.0f, eta = 0.0f;
	aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
	aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_EMISSIVE, &colorEmissive);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_EMISSIVE_INTENSITY, &emissionInt);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_METALLIC_FACTOR, &metallic);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_OPACITY, &opacity);
	aiGetMaterialFloat(aiMaterial, AI_MATKEY_REFRACTI, &eta);
	

	Vec3 glmEmissionColor = Vec3(colorEmissive.r, colorEmissive.g, colorEmissive.b);
	float emissionNormaizer = (float)MaxComponent(glmEmissionColor);
	glmEmissionColor /= emissionNormaizer;
	emissionInt = emissionNormaizer * (emissionInt ? emissionInt : 1.0f);

	std::vector<Buffer2DTexture<Vec3>> diffuseMaps = ProcessAssimpMaterialTextures(aiMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
	std::vector<Buffer2DTexture<Vec3>> specularMaps = ProcessAssimpMaterialTextures(aiMaterial, aiTextureType_SPECULAR, "texture_specular");

	Material material = Material(materialName);
	material.m_albedo = Vec3(color.r, color.g, color.b);
	if (diffuseMaps.size() > 0) {
		material.m_albedoTexture = diffuseMaps[0];
	}
	material.m_roughness = 1.0f - metallic;
	material.m_metallic = metallic;
	if (specularMaps.size() > 0) {

	}
	if (!isnan(glmEmissionColor.r) && !isnan(glmEmissionColor.y) && !isnan(glmEmissionColor.z)) {
		material.m_emissionColor = glmEmissionColor;
	}
	if (!isnan(emissionInt)) {
		material.m_emissionStrength = emissionInt;
	}
	if (!isnan(opacity)) {
		material.m_transparency = 1.0f - opacity;
	}
	if (!isnan(eta)) {
		material.m_refraction = eta;
	}

	return AddMaterial(material);
}

std::vector<Buffer2DTexture<Vec3>> ResourceManager::ProcessAssimpMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name) {
	std::vector<Buffer2DTexture<Vec3>> textures;
	for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		std::filesystem::path texturePath = m_currentFilePath.parent_path().string() + "/" + str.C_Str();
		textures.push_back(LoadBuffer2DTextureRGB(texturePath));
	}
	return textures;
}

std::vector<std::string> ResourceManager::SplitPBRTFileLine(const std::string& line) {
	std::vector<std::string> ss;
	bool quotedLine = false;
	std::string s = "";
	for (int32_t i = 0; i < line.size(); i++) {
		if (line[i] == '\"') {
			quotedLine = !quotedLine;
			continue;
		}
		if (std::isspace(line[i])) {
			if (quotedLine) {
				s += line[i];
			}
			else if (s != "") {
				ss.push_back(s);
				s = "";
			}
			continue;
		}
		s += line[i];
	}
	if (s != "") {
		ss.push_back(s);
	}
	return ss;
}

Float ResourceManager::StringToFloat(const std::string& str) {
	try {
		return std::stof(str);
	}
	catch (std::exception e) {
		return 0.0f;
	}
}

Material* ResourceManager::FindMaterial(const std::string& name) {
	auto it = std::find_if(m_materials.begin(), m_materials.end(), [&](const Material& mat) {
		return mat.m_name == name;
		});
	if (it == m_materials.end()) {
		return nullptr;
	}
	return &(*it);
}

Material* ResourceManager::AddMaterial(const Material& material) {
	if (m_materials.size() == c_maxMaterials) {
		throw new std::exception("Materials limit exceeded.");
	}
	m_materials.push_back(material);
	return &m_materials.back();
}

std::string ResourceManager::ReadFile(const std::string& filePath) {
	std::ifstream t(filePath);
	std::string file((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return file;
}

Shader ResourceManager::LoadShader(const std::string& name) {
	return LoadShader(name + std::string("VertexShader.glsl"), name + std::string("FragmentShader.glsl"));
}

Shader ResourceManager::LoadShader(const std::string& vertexName, const std::string& fragmentName) {
	const std::string shadersPath = GetApplicationDirectory().string() + std::string("/Resources/Shaders/");
	std::string vertShaderStr = ReadFile(shadersPath + vertexName);
	std::string fragShaderStr = ReadFile(shadersPath + fragmentName);
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();
	std::cout << "  Compiling shaders: " << vertexName << " and " << fragmentName << "\n";
	return CompileShader(vertShaderSrc, fragShaderSrc);
}

Shader ResourceManager::CompileShader(const char* vertShaderSrc, const char* fragShaderSrc) {
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int32_t logLength;

	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
	if (logLength) {
		std::cout << &vertShaderError[0] << std::endl;
	}

	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
	if (logLength) {
		std::cout << &fragShaderError[0] << std::endl;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	if (logLength) {
		std::cout << &programError[0] << std::endl;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return Shader(program);
}

ComputeShader ResourceManager::LoadComputeShader(const std::string& computePath) {
	const std::string shadersPath = GetApplicationDirectory().string() + std::string("/Resources/Shaders/");
	std::string computeShaderStr = ReadFile(shadersPath + computePath);
	return CompileComputeShader(computeShaderStr.c_str());
}

ComputeShader ResourceManager::CompileComputeShader(const char* computeShaderSrc) {
	GLint result = GL_FALSE;
	int32_t logLength;
	GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(ray_shader, 1, &computeShaderSrc, NULL);
	glCompileShader(ray_shader);

	glGetShaderiv(ray_shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(ray_shader, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
	glGetShaderInfoLog(ray_shader, logLength, NULL, &fragShaderError[0]);
	std::cout << &fragShaderError[0] << std::endl;

	GLuint rayProgram = glCreateProgram();
	glAttachShader(rayProgram, ray_shader);
	glLinkProgram(rayProgram);

	glGetProgramiv(rayProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(rayProgram, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(rayProgram, logLength, NULL, &programError[0]);
	std::cout << &programError[0] << std::endl;

	return ComputeShader(rayProgram);
}

const FontCharacter& ResourceManager::GetFontCharacter(char c) {
	return m_characters[c];
}

uint32_t ResourceManager::GetDefaultFontSize() {
	return m_defaultFontSize;
}

void ResourceManager::FreeTextures() {

}

void ResourceManager::LoadDefaultFont() {
	const std::string fontsPath = GetApplicationDirectory().string() + std::string("/Resources/Fonts/");
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		//return -1;
	}
	FT_Face face;
	if (FT_New_Face(ft, (fontsPath + std::string("Arial.ttf")).c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		//return -1;
	}
	FT_Set_Pixel_Sizes(face, 0, m_defaultFontSize);
	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		//return -1;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	for (int32_t c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		FontCharacter character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(uint32_t)face->glyph->advance.x
		};
		m_characters.insert(std::pair<char, FontCharacter>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

const Texture& ResourceManager::GetBRDFLookUpTexture() {
	return m_textures["brdfLUT"];
}
