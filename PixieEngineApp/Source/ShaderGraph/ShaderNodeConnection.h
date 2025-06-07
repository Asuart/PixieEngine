#pragma once
#include "pch.h"

struct ShaderNodeInput;
struct ShaderNodeOutput;

struct ShaderNodeConnection {
	ShaderNodeOutput& from;
	ShaderNodeInput& to;

	ShaderNodeConnection(ShaderNodeOutput& _from, ShaderNodeInput& _to);
};
