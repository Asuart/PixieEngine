#pragma once
#include "pch.h"
#include "AssimpGLMHelper.h"
#include "SceneObject.h"

struct KeyPosition {
    Vec3 position = Vec3(0.0f);
    Float timeStamp = 0.0f;
};

struct KeyRotation {
    Quaternion orientation = Quaternion();
    Float timeStamp = 0.0f;
};

struct KeyScale {
    Vec3 scale = Vec3(1.0f);
    Float timeStamp = 0.0f;
};

struct BoneInfo {
    int32_t id = -1;
    Mat4 offset = Mat4(1.0f);
};

struct Bone {
    std::string name;
    int32_t id = -1;
    Mat4 localTransform = Mat4(1.0f);
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;

    Bone(const std::string& name, int32_t ID);

    void Update(Float animationTime);

    int32_t GetPositionIndex(Float animationTime) const;
    int32_t GetRotationIndex(Float animationTime) const;
    int32_t GetScaleIndex(Float animationTime) const;

private:
    Float GetScaleFactor(Float lastTimeStamp, Float nextTimeStamp, Float animationTime);
    Mat4 InterpolatePosition(Float animationTime);
    Mat4 InterpolateRotation(Float animationTime);
    Mat4 InterpolateScaling(Float animationTime);
};

class Animation {
public:

    Animation() = default;
    Animation(float duration, int32_t ticksPerSecond, const std::vector<Bone>& bones, const std::map<std::string, BoneInfo>& boneInfoMap, SceneObject* rootObject);
    ~Animation();

    Bone* FindBone(const std::string& name);

    int32_t GetTicksPerSecond() const;
    float GetDuration() const;
    const SceneObject* GetRootNode() const;
    const std::map<std::string, BoneInfo>& GetBoneIDMap() const;

private:
    float duration;
    int32_t ticksPerSecond;
    std::vector<Bone> bones;
    std::map<std::string, BoneInfo> boneInfoMap;
    SceneObject* rootNode;
};

class Animator {
public:
    Animator(Animation* Animation, Mat4 globalInverseTransform);

    void UpdateAnimation(Float dt);
    void PlayAnimation(Animation* pAnimation);
    std::vector<Mat4> GetFinalBoneMatrices();

private:
    Mat4 m_globalInverseTransform;
    std::vector<Mat4> finalBoneMatrices;
    Animation* currentAnimation = nullptr;
    Float currentTime = 0.0f;
    Float deltaTime = 0.0f;

    void CalculateBoneTransform(const SceneObject* node, Mat4 parentTransform);
};
