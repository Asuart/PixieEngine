#pragma once
#include "ShaderGraph/ShaderNode.h"

class WorldSpaceAmbientOcclusionNode : public ShaderNode {
public:
	WorldSpaceAmbientOcclusionNode();

	void Process(const Scene& scene, const Camera& camera) override;

protected:
	GLuint m_ao;
};
