#pragma once
#include "pch.h"
#include "InterfaceWindow.h"
#include "UserInput.h"
#include "BezierCurves.h"
#include "ShaderGraph/ShaderNode.h"
#include "MeshGenerator.h"

struct ShaderNodeConnectionObject;
struct ShaderNodeObject;

struct ShaderNodeInputObject {
	ShaderNodeObject& parent;
	Vec2 offset = Vec2();
	Vec3 color = Vec3(0.8f, 0.6f, 0.1f);
	ShaderNodeInput& input;
	ShaderNodeConnectionObject* connection = nullptr;

	ShaderNodeInputObject(ShaderNodeObject& _parent, ShaderNodeInput& _input, Vec2 _offset);
};

struct ShaderNodeOutputObject {
	ShaderNodeObject& parent;
	Vec2 offset = Vec2();
	Vec3 color = Vec3(0.8f, 0.6f, 0.1f);
	ShaderNodeOutput& output;
	std::vector<ShaderNodeConnectionObject*> connections;

	ShaderNodeOutputObject(ShaderNodeObject& _parent, ShaderNodeOutput& _output, Vec2 _offset);
};

struct VirtualShaderNodeConnectionObject {
	ShaderNodeOutputObject& from;
	Vec2 to;
	Mesh* mesh = nullptr;

	VirtualShaderNodeConnectionObject(ShaderNodeOutputObject& _from, Vec2 _to);

	void UpdateMesh(Vec2 end);
};

struct ShaderNodeConnectionObject {
	ShaderNodeConnection& connection;
	ShaderNodeOutputObject& from;
	ShaderNodeInputObject& to;
	Mesh* mesh = nullptr;

	ShaderNodeConnectionObject(ShaderNodeConnection& _connection, ShaderNodeOutputObject& _from, ShaderNodeInputObject& _to);

	void UpdateMesh();
};

struct ShaderNodeObject {
	ShaderNode& node;
	Vec2 position = Vec2();
	Vec2 size = Vec2();
	Vec4 baseColor = Vec4(0.15f, 0.15f, 0.15f, 1.0f);
	std::map<const ShaderNodeInput*, ShaderNodeInputObject*> inputs;
	std::map<const ShaderNodeOutput*, ShaderNodeOutputObject*> outputs;

	ShaderNodeObject(ShaderNode& _node, Vec2 _position, Vec2 _size);
	~ShaderNodeObject();
};

struct ShaderGraphObject {
	std::map<ShaderNode*, ShaderNodeObject*> m_nodes;
	std::map<const ShaderNodeConnection*, ShaderNodeConnectionObject*> m_connections;

	ShaderGraphObject(ShaderGraph& graph);
	~ShaderGraphObject();

	Vec2 GetNodeSize(ShaderNode* node);
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
	Vec2 m_viewOffset = { 0,0 };
	Float m_viewScale = 1.0f;
	Mat4 m_projection = Mat4(1.0f);
	Vec2 m_cursorPos = Vec2(0.0f);
	Vec2 m_prevCursorPos = Vec2(0.0f);
	Vec2 m_cursorViewportPosition = Vec2(0.0f);
	Shader m_bezierShader;

	ShaderGraphWindowState m_state = ShaderGraphWindowState::Default;
	ShaderNodeObject* m_grabbedNode = nullptr;
	VirtualShaderNodeConnectionObject* m_grabbedConnection = nullptr;

	static constexpr Float minViewScale = 0.1f;
	static constexpr Float maxViewScale = 10.0f;
	static constexpr Vec2 minViewOffset = Vec2(-1000.0f);
	static constexpr Vec2 maxViewOffset = Vec2(1000.0f);

	void DrawNodes();
	void DrawConnections();
	void HandleUserInput();
	void UpdateProjection();
	void Connect(ShaderNodeOutputObject& from, ShaderNodeInputObject& to);
	void RemoveConnection(ShaderNodeConnectionObject* connection);
};
