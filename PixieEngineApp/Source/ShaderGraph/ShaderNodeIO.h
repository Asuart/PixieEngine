#pragma once
#include "pch.h"

enum class ShaderNodeIOType : int32_t {
	undefined = 0,
	int32 = (1 << 0),
	float32 = (1 << 1),
	vec2 = (1 << 2),
	vec3 = (1 << 3),
	vec4 = (1 << 4),
	textureR = (1 << 5),
	textureRGB = (1 << 6),
	// united
	texture = (textureR | textureRGB),
};

class ShaderNode;
struct ShaderNodeConnection;

struct ShaderNodeIO {
	ShaderNode& m_parent;
	std::string m_name;
	ShaderNodeIOType m_type;
};

struct ShaderNodeInput : public ShaderNodeIO {
	ShaderNodeConnection* m_connection = nullptr;

	ShaderNodeInput(ShaderNode& parent, const std::string& name, ShaderNodeIOType type);
};

struct ShaderNodeOutput : public ShaderNodeIO {
	std::vector<ShaderNodeConnection*> m_connections;
	void* m_value = nullptr;

	ShaderNodeOutput(ShaderNode& parent, const std::string& name, ShaderNodeIOType type, void* value);
};
