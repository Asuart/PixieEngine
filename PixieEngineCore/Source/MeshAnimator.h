#pragma once
#include "PixieEngineCoreHeaders.h"
#include "AssimpGLMHelper.h"

struct BoneInfo {
    int32_t id;
    glm::mat4 offset;
};

struct KeyPosition {
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

class Bone {
public:
    Bone(const std::string& name, int32_t ID, const aiNodeAnim* channel);

    void Update(float animationTime);

    glm::mat4 GetLocalTransform() const;
    std::string GetBoneName() const;
    int32_t GetBoneID() const;

    int32_t GetPositionIndex(float animationTime) const;
    int32_t GetRotationIndex(float animationTime) const;
    int32_t GetScaleIndex(float animationTime) const;

private:
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
    int32_t numPositions;
    int32_t numRotations;
    int32_t numScalings;

    glm::mat4 localTransform;
    std::string name;
    int32_t id;

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime);
};

struct Skeleton {
    std::map<std::string, BoneInfo> boneInfoMap;
    int32_t boneCount;

    std::map<std::string, BoneInfo>& GetBoneInfoMap() {
        return boneInfoMap;
    }

    int32_t& GetBoneCount() {
        return boneCount;
    }
};

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int32_t childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:
    Animation() = default;
    Animation(const std::string& animationPath, Skeleton* model);
    ~Animation();

    Bone* FindBone(const std::string& name);

    float GetTicksPerSecond() const;
    float GetDuration() const;
    const AssimpNodeData& GetRootNode() const;
    const std::map<std::string, BoneInfo>& GetBoneIDMap() const;

private:
    float duration;
    int32_t ticksPerSecond;
    std::vector<Bone> bones;
    AssimpNodeData rootNode;
    std::map<std::string, BoneInfo> boneInfoMap;

    void ReadMissingBones(const aiAnimation* animation, Skeleton& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};

class Animator {
public:
    Animator(Animation* Animation);

    void UpdateAnimation(float dt);
    void PlayAnimation(Animation* pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
    std::vector<glm::mat4> GetFinalBoneMatrices();

private:
    std::vector<glm::mat4> finalBoneMatrices;
    Animation* currentAnimation;
    float currentTime;
    float deltaTime;
};
