#pragma once
#include "PixieEngineCoreHeaders.h"
#include "AssimpGLMHelper.h"

struct KeyPosition {
    Vec3 position;
    Float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    Float timeStamp;
};

struct KeyScale {
    Vec3 scale;
    Float timeStamp;
};

struct BoneInfo {
    int32_t id;
    Mat4 offset;
};

class Bone {
public:
    std::string name;
    int32_t id;
    Mat4 localTransform;
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

struct AssimpNodeData {
    Mat4 transformation;
    std::string name;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:

    Animation() = default;
    Animation(float duration, int32_t ticksPerSecond, SceneObject* rootNode);
    Animation(const std::string& animationPath, Skeleton* model);
    ~Animation();

    Bone* FindBone(const std::string& name);

    int32_t GetTicksPerSecond() const;
    Float GetDuration() const;
    const AssimpNodeData& GetRootNode() const;
    const std::map<std::string, BoneInfo>& GetBoneIDMap() const;

private:
    Float duration;
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

    void UpdateAnimation(Float dt);
    void PlayAnimation(Animation* pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, Mat4 parentTransform);
    std::vector<Mat4> GetFinalBoneMatrices();

private:
    std::vector<Mat4> finalBoneMatrices;
    Animation* currentAnimation;
    Float currentTime;
    Float deltaTime;
};
