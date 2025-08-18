#pragma once
#include "pch.h"
#include "ShaderHandle.h"
#include "MeshHandle.h"
#include "Resources/Mesh.h"

namespace PixieEngine {

class RendererBase {
public:
	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void DrawMesh(MeshHandle mesh) = 0;

	virtual ShaderHandle CreateShader(const std::string& vertexShaderSource, const std::string fragmentShaderShource) = 0;
	virtual MeshHandle LoadMesh(const Mesh& mesh) = 0;
};

}