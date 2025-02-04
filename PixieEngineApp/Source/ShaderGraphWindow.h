#pragma once
#include "InterfaceWindow.h"
#include "UserInput.h"
#include "BezierCurves.h"
#include "ShaderGraph.h"

struct ShaderNodeConnectionObject;
struct ShaderNodeObject;

struct ShaderNodeInObject {
	ShaderNodeObject* parent = nullptr;
	ShaderNodeIn* in = nullptr;
	Vec2 offset = Vec2();
	Vec3 color = Vec3(0.8f, 0.6f, 0.1f);
	ShaderNodeConnectionObject* connection = nullptr;

	ShaderNodeInObject() = default;
	ShaderNodeInObject(ShaderNodeObject* _parent, ShaderNodeIn* _in, Vec2 _offset) :
		parent(_parent), in(_in), offset(_offset) {}
};

struct ShaderNodeOutObject {
	ShaderNodeObject* parent = nullptr;
	ShaderNodeOut* out = nullptr;
	Vec2 offset = Vec2();
	Vec3 color = Vec3(0.8f, 0.6f, 0.1f);
	std::vector<ShaderNodeConnectionObject*> connections;

	ShaderNodeOutObject() = default;
	ShaderNodeOutObject(ShaderNodeObject* _parent, ShaderNodeOut* _out, Vec2 _offset) :
		parent(_parent), out(_out), offset(_offset) {}
};

struct ShaderNodeConnectionObject {
	ShaderNodeConnection* connection = nullptr;
	ShaderNodeOutObject* from = nullptr;
	ShaderNodeInObject* to = nullptr;
	Mesh* mesh = nullptr;

	ShaderNodeConnectionObject(ShaderNodeConnection* _connection, ShaderNodeOutObject* _from, ShaderNodeInObject* _to) :
		connection(_connection), from(_from), to(_to) {
		UpdateMesh();
	}

	void UpdateMesh();
	void UpdateTempMesh(Vec2 end);
};

const Float c_nodeBodyStart = 30;
const Float c_nodeRowHeight = 24;

struct ShaderNodeObject {
	ShaderNode* node = nullptr;
	Vec2 position = Vec2();
	Vec2 size = Vec2();
	Vec4 baseColor = Vec4(0.15f, 0.15f, 0.15f, 1.0f);
	std::map<ShaderNodeIn*, ShaderNodeInObject*> inputs;
	std::map<ShaderNodeOut*, ShaderNodeOutObject*> outputs;

	ShaderNodeObject() = default;
	ShaderNodeObject(ShaderNode* _node, Vec2 _position, Vec2 _size) :
		node(_node), position(_position), size(_size) {
		int32_t row = 0;
		for (int32_t i = 0; i < node->m_inputs.size(); i++, row++) {
			inputs.insert({ &node->m_inputs[i], new ShaderNodeInObject(this, &node->m_inputs[i], Vec2(-4, c_nodeBodyStart + c_nodeRowHeight * row)) });
		}
		for (int32_t i = 0; i < node->m_outputs.size(); i++, row++) {
			outputs.insert({ &node->m_outputs[i], new ShaderNodeOutObject(this, &node->m_outputs[i], Vec2(size.x - 4, c_nodeBodyStart + c_nodeRowHeight * row)) });
		}
	}
	~ShaderNodeObject() {
		for (auto pair : inputs) {
			delete pair.second;
		}
		for (auto pair : outputs) {
			delete pair.second;
		}
	}
};

struct ShaderGraphObject {
	ShaderGraphObject(const ShaderGraph& graph) {
		for (int32_t i = 0; i < graph.m_nodes.size(); i++) {
			m_nodes.insert({ graph.m_nodes[i], new ShaderNodeObject(graph.m_nodes[i], Vec2(100 + 300 * i, 100), GetNodeSize(graph.m_nodes[i])) });
		}
		for (int32_t i = 0; i < graph.m_connections.size(); i++) {
			ShaderNodeConnectionObject* connection = new ShaderNodeConnectionObject(
				graph.m_connections[i],
				m_nodes[graph.m_connections[i]->from->parent]->outputs[graph.m_connections[i]->from],
				m_nodes[graph.m_connections[i]->to->parent]->inputs[graph.m_connections[i]->to]
			);
			m_nodes[graph.m_connections[i]->from->parent]->outputs[graph.m_connections[i]->from]->connections.push_back(connection);
			m_nodes[graph.m_connections[i]->to->parent]->inputs[graph.m_connections[i]->to]->connection = connection;
			m_connections.insert({ graph.m_connections[i], connection });
		}
	}
	~ShaderGraphObject() {
		for (auto pair : m_nodes) {
			delete pair.second;
		}
		for (auto pair : m_connections) {
			delete pair.second;
		}
	}

	Vec2 GetNodeSize(ShaderNode* node) {
		return { 200, c_nodeBodyStart + (node->m_inputs.size() + node->m_outputs.size()) * c_nodeRowHeight };
	}

	std::map<ShaderNode*, ShaderNodeObject*> m_nodes;
	std::map<ShaderNodeConnection*, ShaderNodeConnectionObject*> m_connections;
};

enum class ShaderGraphWindowState {
	Default = 0,
	DragNode,
	ConnectingNodes
};

class ShaderGraphWindow : public InterfaceWindow {
public:
	ShaderGraphWindow(PixieEngineApp& app, Interface& inter)
		: InterfaceWindow(app, inter), m_frameBuffer({ 1280, 720 }), m_shaderGraphObject(m_shaderGraph) {
		m_bezierShader = ResourceManager::LoadShader("Bezier2DVertexShader.glsl", "Bezier2DFragmentShader.glsl");
	}

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
	ShaderNodeConnectionObject* m_grabbedConnection = nullptr;

	static constexpr Float minViewScale = 0.1f;
	static constexpr Float maxViewScale = 10.0f;
	static constexpr Vec2 minViewOffset = Vec2(-1000.0f);
	static constexpr Vec2 maxViewOffset = Vec2(1000.0f);

	void DrawNodes() {
		for (auto& pair : m_shaderGraphObject.m_nodes) {
			ShaderNodeObject* obj = pair.second;
			GlobalRenderer::DrawUIBox(obj->position, obj->size, obj->baseColor, 5.0f, 0.0f, Vec4(0.0f), m_projection);
			GlobalRenderer::DrawText(obj->node->m_name, obj->position + Vec2(8.0f, 16.0f), 16.0f, Vec3(1.0f), m_projection);
			for (auto& pair : obj->inputs) {
				ShaderNodeInObject* in = pair.second;
				GlobalRenderer::DrawUIBox(obj->position + in->offset, { 8, 8 }, Vec4(1.0f, 0.15f, 0.3f, 1.0f), 100.0f, 0.0f, Vec4(0.0f), m_projection);
				GlobalRenderer::DrawText(in->in->name, obj->position + in->offset + Vec2(12.0f, 8.0f), 14.0f, Vec3(1.0f), m_projection);
			}
			for (auto& pair : obj->outputs) {
				ShaderNodeOutObject* out = pair.second;
				GlobalRenderer::DrawUIBox(obj->position + out->offset, { 8, 8 }, Vec4(1.0f, 0.15f, 0.3f, 1.0f), 100.0f, 0.0f, Vec4(0.0f), m_projection);
				GlobalRenderer::DrawText(out->out->name, obj->position + out->offset + Vec2(-4.0f, 8.0f), 14.0f, Vec3(1.0f), m_projection, true);
			}
		}
	}

	void DrawConnections() {
		m_bezierShader.Bind();
		m_bezierShader.SetUniformMat4f("mProjection", m_projection);
		for (auto& pair : m_shaderGraphObject.m_connections) {
			pair.second->mesh->Draw();
		}
		if (m_grabbedConnection) {
			m_grabbedConnection->UpdateTempMesh(m_cursorPos);
			m_grabbedConnection->mesh->Draw();
		}
	}

	void HandleUserInput() {
		if (m_state == ShaderGraphWindowState::Default) {
			// Move viewport
			if (UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_3)) {
				m_viewOffset -= Vec2(UserInput::mouseDeltaX, UserInput::mouseDeltaY) * m_viewScale;
				m_viewOffset = glm::clamp(m_viewOffset, minViewOffset, maxViewOffset);
				return;
			}
			// Scele viewport
			if (UserInput::mouseScrollY) {
				Float initilScale = m_viewScale;
				m_viewScale -= UserInput::mouseScrollY * 0.1f;
				m_viewScale = glm::clamp(m_viewScale, minViewScale, maxViewScale);
				m_viewOffset += (m_cursorViewportPosition) * (initilScale - m_viewScale);
				return;
			}
			if (UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_1)) {
				for (auto& pair : m_shaderGraphObject.m_nodes) {
					ShaderNodeObject* node = pair.second;

					// Grab Node
					if (m_cursorPos.x > node->position.x && m_cursorPos.x < node->position.x + node->size.x && m_cursorPos.y > node->position.y && m_cursorPos.y < node->position.y + 20) {
						m_grabbedNode = node;
						m_state = ShaderGraphWindowState::DragNode;
						break;
					}
					
					// Grab connection
					bool grabbedConnection = false;
					for (auto pair : node->inputs) {
						ShaderNodeInObject* input = pair.second;
						if (!input->connection) continue;
						Vec2 offset = m_cursorPos - node->position;
						if (offset.x > input->offset.x && offset.x < input->offset.x + 8 && offset.y > input->offset.y && offset.y < input->offset.y + 8) {
							m_grabbedConnection = new ShaderNodeConnectionObject(nullptr, input->connection->from, nullptr);
							m_grabbedConnection->UpdateTempMesh(m_cursorPos);
							RemoveConnection(input->connection);
							m_state = ShaderGraphWindowState::ConnectingNodes;
							grabbedConnection = true;
							break;
						}
					}
					if (grabbedConnection) break;
					for (auto pair : node->outputs) {
						ShaderNodeOutObject* output = pair.second;
						Vec2 offset = m_cursorPos - node->position;
						if (offset.x > output->offset.x && offset.x < output->offset.x + 8 && offset.y > output->offset.y && offset.y < output->offset.y + 8) {
							m_grabbedConnection = new ShaderNodeConnectionObject(nullptr, output, nullptr);
							m_grabbedConnection->UpdateTempMesh(m_cursorPos);
							m_state = ShaderGraphWindowState::ConnectingNodes;
							grabbedConnection = true;
							break;
						}
					}
					if (grabbedConnection) break;
				}
			}
		}
		else if (m_state == ShaderGraphWindowState::DragNode) {
			if (!m_grabbedNode || !UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_1)) {
				m_state = ShaderGraphWindowState::Default;
				m_grabbedNode = nullptr;
				return;
			}
			m_grabbedNode->position -= m_prevCursorPos - m_cursorPos;
			for (auto& pair : m_grabbedNode->inputs) {
				ShaderNodeInObject* in = pair.second;
				if (in->connection) {
					in->connection->UpdateMesh();
				}
			}
			for (auto& pair : m_grabbedNode->outputs) {
				ShaderNodeOutObject* out = pair.second;
				for (int32_t j = 0; j < out->connections.size(); j++) {
					out->connections[j]->UpdateMesh();
				}
			}
		}
		else if (m_state == ShaderGraphWindowState::ConnectingNodes) {
			// Move viewport
			if (UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_3)) {
				m_viewOffset -= Vec2(UserInput::mouseDeltaX, UserInput::mouseDeltaY) * m_viewScale;
				m_viewOffset = glm::clamp(m_viewOffset, minViewOffset, maxViewOffset);
				return;
			}
			// Scale viewport
			if (UserInput::mouseScrollY) {
				Float initilScale = m_viewScale;
				m_viewScale -= UserInput::mouseScrollY * 0.1f;
				m_viewScale = glm::clamp(m_viewScale, minViewScale, maxViewScale);
				m_viewOffset += (m_cursorViewportPosition) * (initilScale - m_viewScale);
				return;
			}
			// Try to connect nodes
			if (!UserInput::GetMouseButton(GLFW_MOUSE_BUTTON_1)) {
				for (auto& pair : m_shaderGraphObject.m_nodes) {
					ShaderNodeObject* node = pair.second;
					bool connected = false;
					for (auto pair : node->inputs) {
						ShaderNodeInObject* input = pair.second;
						Vec2 offset = m_cursorPos - node->position;
						if (offset.x > input->offset.x && offset.x < input->offset.x + 8 && offset.y > input->offset.y && offset.y < input->offset.y + 8) {
							Connect(m_grabbedConnection->from, input);
							connected = true;
							break;
						}
					}
					if (connected) break;
				}
				m_state = ShaderGraphWindowState::Default;
				delete m_grabbedConnection;
				m_grabbedConnection = nullptr;
			}
		}
	}

	void UpdateProjection() {
		m_projection = glm::ortho(m_viewOffset.x, m_viewOffset.x + m_frameBuffer.m_resolution.x * m_viewScale, m_viewOffset.y + m_frameBuffer.m_resolution.y * m_viewScale, m_viewOffset.y);
	}

	void Connect(ShaderNodeOutObject* from, ShaderNodeInObject* to) {
		ShaderNodeConnection* connection = m_shaderGraph.Connect(from->out, to->in);
		ShaderNodeConnectionObject* connectionObject = new ShaderNodeConnectionObject(connection,
			m_shaderGraphObject.m_nodes[connection->from->parent]->outputs[connection->from],
			m_shaderGraphObject.m_nodes[connection->to->parent]->inputs[connection->to]);
		m_shaderGraphObject.m_connections[connection] = connectionObject;
		m_shaderGraphObject.m_nodes[connection->from->parent]->outputs[connection->from]->connections.push_back(connectionObject);
		if (m_shaderGraphObject.m_nodes[connection->to->parent]->inputs[connection->to]->connection) {
			RemoveConnection(m_shaderGraphObject.m_nodes[connection->to->parent]->inputs[connection->to]->connection);
		}
		m_shaderGraphObject.m_nodes[connection->to->parent]->inputs[connection->to]->connection = connectionObject;
	}

	void RemoveConnection(ShaderNodeConnectionObject* connection) {
		ShaderNodeConnection* c = connection->connection;

		connection->to->connection = nullptr;
		for (int32_t i = 0; i < connection->from->connections.size(); i++) {
			if (connection->from->connections[i] == connection) {
				connection->from->connections.erase(connection->from->connections.begin() + i);
				break;
			}
		}
		delete connection;
		m_shaderGraphObject.m_connections.erase(c);

		c->to->connection = nullptr;
		for (int32_t i = 0; i < c->from->connections.size(); i++) {
			if (c->from->connections[i] == c) {
				c->from->connections.erase(c->from->connections.begin() + i);
				break;
			}
		}
		delete c;
		for (int32_t i = 0; i < m_shaderGraph.m_connections.size(); i++) {
			if (m_shaderGraph.m_connections[i] == c) {
				m_shaderGraph.m_connections.erase(m_shaderGraph.m_connections.begin() + i);
				break;
			}
		}
	}
};
