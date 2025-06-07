#include "pch.h"
#include "DisplayFrameNode.h"

DisplayFrameNode::DisplayFrameNode() :
	ShaderNode("Display Frame") {
	m_inputs.push_back({ *this, "Frame", ShaderNodeIOType::texture });
}
