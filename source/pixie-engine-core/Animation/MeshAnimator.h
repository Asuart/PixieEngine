#pragma once
#include "pch.h"
#include "Scene/SceneObject.h"

namespace PixieEngine {

const int32_t cMaxBonesPerModel = 256;

struct KeyPosition {
    glm::vec3 position = glm::vec3(0.0f);
    float timeStamp = 0.0f;
};

struct KeyRotation {
    glm::quat orientation = glm::quat();
    float timeStamp = 0.0f;
};

struct KeyScale {
    glm::vec3 scale = glm::vec3(1.0f);
    float timeStamp = 0.0f;
};

struct BoneInfo {
    int32_t id = -1;
    glm::mat4 offset = glm::mat4(1.0f);
};

struct Bone {
    std::string name;
    int32_t id = -1;
    glm::mat4 localTransform = glm::mat4(1.0f);
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;

    Bone(const std::string& name, int32_t ID);

    void Update(float animationTime);

    int32_t GetPositionIndex(float animationTime) const;
    int32_t GetRotationIndex(float animationTime) const;
    int32_t GetScaleIndex(float animationTime) const;

private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime);
};

class Animation {
public:

    Animation() = default;
    Animation(float duration, int32_t ticksPerSecond, const std::vector<Bone>& bones, const std::map<std::string, BoneInfo>& boneInfoMap, SceneObject* rootObject);
    ~Animation();

    Bone* FindBone(const std::string& name);

    int32_t GetTicksPerSecond() const;
    float GetDuration() const;
    SceneObject* GetRootNode();
    std::map<std::string, BoneInfo>& GetBoneIDMap();

private:
    float duration;
    int32_t ticksPerSecond;
    std::vector<Bone> bones;
    std::map<std::string, BoneInfo> boneInfoMap;
    SceneObject* rootNode;
};

class Animator {
public:
    Animator(Animation* Animation, glm::mat4 globalInverseTransform);

    void PlayAnimation(Animation* pAnimation);
    void UpdateAnimation(float dt);
    const std::array<glm::mat4, cMaxBonesPerModel>* GetFinalBoneMatrices() const;

private:
    glm::mat4 m_globalInverseTransform;
    std::array<glm::mat4, cMaxBonesPerModel> m_finalBoneMatrices;
    Animation* m_currentAnimation = nullptr;
    float m_currentTime = 0.0f;
    float m_deltaTime = 0.0f;

    void CalculateBoneTransform(SceneObject* node, glm::mat4 parentTransform);
};

}