#include "pch.h"
#include "ShaderGraph.h"

ShaderGraph::ShaderGraph() {
	m_nodes.push_back(new DefferedRenderNode());
	m_nodes.push_back(new WorldSpaceAmbientOcclusionNode());
	m_nodes.push_back(new DefferedLightingNode());
	m_nodes.push_back(new DisplayFrameNode());

	// Deffered Render to World Space AO
	Connect(m_nodes[0]->GetOutput("Position"), m_nodes[1]->GetInput("Position"));
	Connect(m_nodes[0]->GetOutput("Normal"), m_nodes[1]->GetInput("Normal"));
	Connect(m_nodes[0]->GetOutput("Depth"), m_nodes[1]->GetInput("Depth"));

	// Deffered Render to Deffered Lighting
	Connect(m_nodes[0]->GetOutput("Albedo"), m_nodes[2]->GetInput("Albedo"));
	Connect(m_nodes[0]->GetOutput("Normal"), m_nodes[2]->GetInput("Normal"));
	Connect(m_nodes[0]->GetOutput("Position"), m_nodes[2]->GetInput("Position"));
	Connect(m_nodes[0]->GetOutput("Specular"), m_nodes[2]->GetInput("Specular"));
	Connect(m_nodes[0]->GetOutput("Metallic"), m_nodes[2]->GetInput("Metallic"));
	Connect(m_nodes[0]->GetOutput("Roughness"), m_nodes[2]->GetInput("Roughness"));

	// World Space AO to Deffered Lighting
	Connect(m_nodes[1]->GetOutput("AO"), m_nodes[2]->GetInput("AO"));

	// Deffered lighting to Display Frame
	Connect(m_nodes[2]->GetOutput("Frame"), m_nodes[3]->GetInput("Frame"));
}

ShaderGraph::~ShaderGraph() {
	for (int32_t i = 0; i < m_connections.size(); i++) {
		delete m_connections[i];
	}
	for (int32_t i = 0; i < m_nodes.size(); i++) {
		delete m_nodes[i];
	}
}

const std::vector<ShaderNode*>& ShaderGraph::GetNodes() {
	return m_nodes;
}

const std::vector<ShaderNodeConnection*> ShaderGraph::GetConnections() {
	return m_connections;
}

ShaderNodeConnection* ShaderGraph::Connect(ShaderNodeOutput* from, ShaderNodeInput* to) {
	ShaderNodeConnection* connection = new ShaderNodeConnection(*from, *to);
	if (to->m_connection) {
		RemoveConnection(to->m_connection);
	}
	from->m_connections.push_back(connection);
	to->m_connection = connection;
	m_connections.push_back(connection);
	return connection;
}

void ShaderGraph::RemoveConnection(ShaderNodeConnection* connection) {
	connection->to.m_connection = nullptr;
	for (int32_t i = 0; i < connection->from.m_connections.size(); i++) {
		if (connection->from.m_connections[i] == connection) {
			connection->from.m_connections.erase(connection->from.m_connections.begin() + i);
			break;
		}
	}
	delete connection;
}

void ShaderGraph::Process(const Scene& scene, const Camera& camera) {
	for (size_t i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->Process(scene, camera);
	}
}
