#pragma once
#include "pch.h"
#include "Scene.h"
#include "MeshComponent.h"
#include "MeshAnimator.h"
#include "MeshAnimatorComponent.h"
#include "Shader.h"

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

class ResourceManager {
public:
	static void SetApplicationPath(const std::string& path);
	static std::filesystem::path GetApplicationPath();
	static std::filesystem::path GetApplicationDirectory();
	static std::filesystem::path GetAssetsPath();
	static void SetAssetsPath(std::filesystem::path path);
	static void Initialize();
	static std::shared_ptr<Scene> LoadScene(std::filesystem::path filePath);
	static bool SaveScene(std::filesystem::path filePath, Scene* scene);
	static SceneObject* LoadModel(std::filesystem::path filePath);
	static Texture<Vec3>* LoadRGBTexture(std::filesystem::path filePath);
	static Texture<Vec4>* LoadRGBATexture(std::filesystem::path filePath);
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
	static std::string ReadFile(const std::string& filePath);
	static Shader LoadShader(const std::string& vertexName, const std::string& fragmentName);
	static Shader CompileShader(const char* vertexSource, const char* fragmentSource);
	static ComputeShader LoadComputeShader(const std::string& computePath);
	static ComputeShader CompileComputeShader(const char* computeSource);

protected:
	static std::filesystem::path m_applicationPath;
	static std::filesystem::path m_currentFilePath;
	static std::filesystem::path m_assetsPath;
	static std::map<std::filesystem::path, SceneObject*> m_Models;
	static std::map<std::filesystem::path, Texture<Vec3>*> m_RGBTextures;
	static std::map<std::filesystem::path, Texture<Vec4>*> m_RGBATextures;
	static std::vector<Material> m_materials;
	static std::vector<Mesh*> m_meshes;

	static bool CheckFileExtensionSupport(std::filesystem::path filePath, ResourceType type);
	static std::shared_ptr<Scene> LoadPixieEngineScene(std::filesystem::path path);
	static std::shared_ptr<Scene> LoadPBRTScene(std::filesystem::path filePath);
	static SceneObject* ProcessAssimpNode(const aiScene* scene, const aiNode* node, std::map<std::string, BoneInfo>& boneInfoMap);
	static std::vector<Bone> ProcessAssimpAnimation(const aiAnimation* animation, std::map<std::string, BoneInfo>& boneInfoMap);
	static Mesh* ProcessAssimpMesh(const aiMesh* mesh, std::map<std::string, BoneInfo>& boneInfoMap);
	static Material* ProcessAssimpMaterial(const aiMaterial* material);
	static std::vector<Texture<Vec3>*> ProcessAssimpMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& name);
	static std::vector<std::string> SplitPBRTFileLine(const std::string& line);
	static Float StringToFloat(const std::string& str);
	static Material* FindMaterial(const std::string& name);
};
