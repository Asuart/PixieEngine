#pragma once
#include "pch.h"
#include "Scene/Scene.h"
#include "Animation/MeshAnimator.h"
#include "Shader.h"
#include "Buffer2DTexture.h"

enum class ResourceType : int32_t {
	Scene,
	Model,
	RGBTexture
};

class AssimpGLMHelpers {
public:
	static Mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);
	static Vec2 GetGLMVec(const aiVector2D& vec);
	static Vec3 GetGLMVec(const aiVector3D& vec);
	static Quaternion GetGLMQuat(const aiQuaternion& pOrientation);
};

struct FontCharacter {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	uint32_t advance;
};

class ResourceManager {
public:
	static void SetApplicationPath(const std::string& path);
	static std::filesystem::path GetApplicationPath();
	static std::filesystem::path GetApplicationDirectory();
	static std::filesystem::path GetAssetsPath();
	static void SetAssetsPath(const std::filesystem::path& path);
	static void Initialize();
	static std::shared_ptr<Scene> LoadScene(const std::filesystem::path& filePath);
	static bool SaveScene(const std::filesystem::path& filePath, Scene* scene);
	static SceneObject* LoadModel(const std::filesystem::path& filePath);
	static Texture LoadTextureFloat(const std::filesystem::path& filePath);
	static Texture LoadTexture(const std::filesystem::path& filePath);
	static Texture LoadTextureRGB(const std::filesystem::path& filePath);
	static Texture LoadTextureRGBA(const std::filesystem::path& filePath);
	static Buffer2DTexture<Float> LoadBuffer2DTextureFloat(const std::filesystem::path& filePath);
	static Buffer2DTexture<Vec3> LoadBuffer2DTextureRGB(const std::filesystem::path& filePath);
	static Buffer2DTexture<Vec4> LoadBuffer2DTextureRGBA(const std::filesystem::path& filePath);
	static HDRISkybox LoadSkybox(const std::filesystem::path& path);
	static Material* GetDefaultMaterial();
	static Material* AddMaterial(const Material& material);
	static Material* GetMaterial(uint32_t index);
	static int32_t GetMaterialIndex(const Material* material);
	static int32_t GetMaterialIndex(const std::string& name);
	static std::vector<Material>& GetMaterials();
	static bool IsValidScenePath(const std::filesystem::path& filePath);
	static bool IsValidModelPath(const std::filesystem::path& filePath);
	static bool IsValidTexturePath(const std::filesystem::path& filePath);
	static Mesh* GetQuadMesh();
	static Mesh* GetCubeMesh();
	static Mesh* GetSphereMesh();
	static std::string ReadFile(const std::string& filePath);
	static Shader LoadShader(const std::string& name);
	static Shader LoadShader(const std::string& vertexName, const std::string& fragmentName);
	static Shader CompileShader(const char* vertexSource, const char* fragmentSource);
	static ComputeShader LoadComputeShader(const std::string& computePath);
	static ComputeShader CompileComputeShader(const char* computeSource);
	static const FontCharacter& GetFontCharacter(char c);
	static uint32_t GetDefaultFontSize();
	static const Texture& GetBRDFLookUpTexture();
	static void FreeTextures();

protected:
	static std::filesystem::path m_applicationPath;
	static std::filesystem::path m_currentFilePath;
	static std::filesystem::path m_assetsPath;
	static std::map<std::filesystem::path, SceneObject*> m_models;
	static std::map<std::filesystem::path, Texture> m_textures;
	static std::vector<Material> m_materials;
	static std::vector<Mesh*> m_meshes;
	static std::map<char, FontCharacter> m_characters;
	static uint32_t m_defaultFontSize;

	static bool CheckFileExtensionSupport(const std::filesystem::path& filePath, ResourceType type);
	static std::shared_ptr<Scene> LoadPixieEngineScene(const std::filesystem::path& path);
	static std::shared_ptr<Scene> LoadPBRTScene(const std::filesystem::path& filePath);
	static SceneObject* ProcessAssimpNode(const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap);
	static std::vector<Bone> ProcessAssimpAnimation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap);
	static Mesh* ProcessAssimpMesh(const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap);
	static Material* ProcessAssimpMaterial(const aiMaterial* material);
	static std::vector<Buffer2DTexture<Vec3>> ProcessAssimpMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name);
	static std::vector<std::string> SplitPBRTFileLine(const std::string& line);
	static Float StringToFloat(const std::string& str);
	static Material* FindMaterial(const std::string& name);
	static void LoadDefaultFont();
};
