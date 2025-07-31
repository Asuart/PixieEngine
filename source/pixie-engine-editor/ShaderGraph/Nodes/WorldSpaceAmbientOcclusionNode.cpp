#include "pch.h"
#include "WorldSpaceAmbientOcclusionNode.h"

WorldSpaceAmbientOcclusionNode::WorldSpaceAmbientOcclusionNode() :
	ShaderNode("World Space AO") {
	m_inputs.push_back({ *this, "Normal", ShaderNodeIOType::textureRGB });
	m_inputs.push_back({ *this, "Position", ShaderNodeIOType::textureRGB });
	m_inputs.push_back({ *this, "Depth", ShaderNodeIOType::textureR });

	m_outputs.push_back({ *this, "AO", ShaderNodeIOType::textureR, &m_ao });
}

void WorldSpaceAmbientOcclusionNode::Process(const Scene& scene, const Camera& camera) {

}
