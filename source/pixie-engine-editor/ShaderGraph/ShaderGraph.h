#pragma once
#include "pch.h"
#include "ShaderNodeConnection.h"
#include "Nodes/Nodes.h"

class ShaderGraph {
public:
	ShaderGraph();
	~ShaderGraph();

	const std::vector<ShaderNode*>& GetNodes();
	const std::vector<ShaderNodeConnection*> GetConnections();

	ShaderNodeConnection* Connect(ShaderNodeOutput* from, ShaderNodeInput* to);
	void RemoveConnection(ShaderNodeConnection* connection);
	void Process(const Scene& scene, const Camera& camera);

protected:
	std::vector<ShaderNode*> m_nodes;
	std::vector<ShaderNodeConnection*> m_connections;
};
