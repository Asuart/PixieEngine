#include "pch.h"
#include "ShaderNodeIO.h"
#include "ShaderNode.h"

ShaderNodeInput::ShaderNodeInput(ShaderNode& parent, const std::string& name, ShaderNodeIOType type) :
	ShaderNodeIO({ parent, name, type }) {}

ShaderNodeOutput::ShaderNodeOutput(ShaderNode& parent, const std::string& name, ShaderNodeIOType type, void* value) :
	ShaderNodeIO({ parent, name, type }), m_value(value) {}
