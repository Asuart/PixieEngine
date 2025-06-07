#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "UserInput.h"
#include "Resources/MeshGenerator.h"
#include "ShaderGraph/ShaderNodeIO.h"
#include "ShaderGraph/ShaderGraph.h"

struct ShaderNodeConnectionObject;
struct ShaderNodeObject;

struct ShaderNodeInputObject {
	ShaderNodeObject& parent;
	glm::vec2 offset = glm::vec2();
	glm::vec3 color = glm::vec3(0.8f, 0.6f, 0.1f);
	ShaderNodeInput& input;
	ShaderNodeConnectionObject* connection = nullptr;

	ShaderNodeInputObject(ShaderNodeObject& _parent, ShaderNodeInput& _input, glm::vec2 _offset);
};

struct ShaderNodeOutputObject {
	ShaderNodeObject& parent;
	glm::vec2 offset = glm::vec2();
	glm::vec3 color = glm::vec3(0.8f, 0.6f, 0.1f);
	ShaderNodeOutput& output;
	std::vector<ShaderNodeConnectionObject*> connections;

	ShaderNodeOutputObject(ShaderNodeObject& _parent, ShaderNodeOutput& _output, glm::vec2 _offset);
};

struct VirtualShaderNodeConnectionObject {
	ShaderNodeOutputObject& from;
	glm::vec2 to;
	MeshHandle mesh;

	VirtualShaderNodeConnectionObject(ShaderNodeOutputObject& _from, glm::vec2 _to);

	void UpdateMesh(glm::vec2 end);
};

struct ShaderNodeConnectionObject {
	ShaderNodeConnection& connection;
	ShaderNodeOutputObject& from;
	ShaderNodeInputObject& to;
	MeshHandle mesh;

	ShaderNodeConnectionObject(ShaderNodeConnection& _connection, ShaderNodeOutputObject& _from, ShaderNodeInputObject& _to);

	void UpdateMesh();
};

struct ShaderNodeObject {
	ShaderNode& node;
	glm::vec2 position = glm::vec2();
	glm::vec2 size = glm::vec2();
	glm::vec4 baseColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	std::map<const ShaderNodeInput*, ShaderNodeInputObject*> inputs;
	std::map<const ShaderNodeOutput*, ShaderNodeOutputObject*> outputs;

	ShaderNodeObject(ShaderNode& _node, glm::vec2 _position, glm::vec2 _size);
	~ShaderNodeObject();
};

struct ShaderGraphObject {
	std::map<ShaderNode*, ShaderNodeObject*> m_nodes;
	std::map<const ShaderNodeConnection*, ShaderNodeConnectionObject*> m_connections;

	ShaderGraphObject(ShaderGraph& graph);
	~ShaderGraphObject();

	glm::vec2 GetNodeSize(ShaderNode* node);
};

enum class ShaderGraphWindowState {
	Default = 0,
	DragNode,
	ConnectingNodes
};

class ShaderGraphWindow : public InterfaceWindow {
public:
	ShaderGraphWindow(PixieEngineApp& app, Interface& inter);

	void Draw() override;

protected:
	FrameBuffer m_frameBuffer;
	ShaderGraph m_shaderGraph;
	ShaderGraphObject m_shaderGraphObject;
	glm::vec2 m_viewOffset = glm::vec2(0.0f);
	float m_viewScale = 1.0f;
	glm::mat4 m_projection = glm::mat4(1.0f);
	glm::vec2 m_cursorPos = glm::vec2(0.0f);
	glm::vec2 m_prevCursorPos = glm::vec2(0.0f);
	glm::vec2 m_cursorViewportPosition = glm::vec2(0.0f);
	Shader m_bezierShader;

	ShaderGraphWindowState m_state = ShaderGraphWindowState::Default;
	ShaderNodeObject* m_grabbedNode = nullptr;
	VirtualShaderNodeConnectionObject* m_grabbedConnection = nullptr;

	static constexpr float minViewScale = 0.1f;
	static constexpr float maxViewScale = 10.0f;
	static constexpr glm::vec2 minViewOffset = glm::vec2(-1000.0f);
	static constexpr glm::vec2 maxViewOffset = glm::vec2(1000.0f);

	void DrawNodes();
	void DrawConnections();
	void HandleUserInput();
	void UpdateProjection();
	void Connect(ShaderNodeOutputObject& from, ShaderNodeInputObject& to);
	void RemoveConnection(ShaderNodeConnectionObject* connection);
};
