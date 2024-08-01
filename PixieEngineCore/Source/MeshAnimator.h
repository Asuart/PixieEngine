#pragma once
#include "PixieEngineCoreHeaders.h"
#include "AssimpGLMHelper.h"

struct BoneInfo {
    int32_t id;
    Mat4 offset;
};

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

class Bone {
public:
    Bone(const std::string& name, int32_t ID, const aiNodeAnim* channel);

    void Update(Float animationTime);

    Mat4 GetLocalTransform() const;
    std::string GetBoneName() const;
    int32_t GetBoneID() const;

    int32_t GetPositionIndex(Float animationTime) const;
    int32_t GetRotationIndex(Float animationTime) const;
    int32_t GetScaleIndex(Float animationTime) const;

private:
    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
    int32_t numPositions;
    int32_t numRotations;
    int32_t numScalings;

    Mat4 localTransform;
    std::string name;
    int32_t id;

    Float GetScaleFactor(Float lastTimeStamp, Float nextTimeStamp, Float animationTime);
    Mat4 InterpolatePosition(Float animationTime);
    Mat4 InterpolateRotation(Float animationTime);
    Mat4 InterpolateScaling(Float animationTime);
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
    Mat4 transformation;
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
