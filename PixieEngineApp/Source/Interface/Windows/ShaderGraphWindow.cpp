#include "pch.h"
#include "ShaderGraphWindow.h"
#include "Interface.h"
#include "ViewportWindow.h"

const Float c_nodeBodyStart = 30;
const Float c_nodeRowHeight = 24;

ShaderNodeInputObject::ShaderNodeInputObject(ShaderNodeObject& _parent, ShaderNodeInput& _input, Vec2 _offset) :
	parent(_parent), input(_input), offset(_offset) {}

ShaderNodeOutputObject::ShaderNodeOutputObject(ShaderNodeObject& _parent, ShaderNodeOutput& _output, Vec2 _offset) :
	parent(_parent), output(_output), offset(_offset) {}

VirtualShaderNodeConnectionObject::VirtualShaderNodeConnectionObject(ShaderNodeOutputObject& _from, Vec2 _to) :
	from(_from), to(_to) {
	UpdateMesh(to);
}

void VirtualShaderNodeConnectionObject::UpdateMesh(Vec2 end) {
	to = end;
	Vec2 p0 = from.parent.position + from.offset + Vec2(0, 4);
	Vec2 p1 = p0 + Vec2(50, 0);
	Vec2 p3 = to;
	Vec2 p2 = p3 - Vec2(50, 0);
	if (mesh) delete mesh;
	mesh = MeshGenerator::BezierMesh(BezierCurve2D(p0, p1, p2, p3), 4.0f, 32);
	mesh->Upload();
}

ShaderNodeConnectionObject::ShaderNodeConnectionObject(ShaderNodeConnection& _connection, ShaderNodeOutputObject& _from, ShaderNodeInputObject& _to) :
	connection(_connection), from(_from), to(_to) {
	UpdateMesh();
}

void ShaderNodeConnectionObject::UpdateMesh() {
	Vec2 p0 = from.parent.position + from.offset + Vec2(0, 4);
	Vec2 p1 = p0 + Vec2(50, 0);
	Vec2 p3 = to.parent.position + to.offset + Vec2(4, 4);
	Vec2 p2 = p3 - Vec2(50, 0);
	if (mesh) delete mesh;
	mesh = MeshGenerator::BezierMesh(BezierCurve2D(p0, p1, p2, p3), 4.0f, 32);
	mesh->Upload();
}

ShaderNodeObject::ShaderNodeObject(ShaderNode& _node, Vec2 _position, Vec2 _size) :
	node(_node), position(_position), size(_size) {
	int32_t row = 0;
	for (int32_t i = 0; i < node.GetInputs().size(); i++, row++) {
		inputs.insert({ &node.GetInputs()[i], new ShaderNodeInputObject(*this, node.GetInputs()[i], Vec2(-4, c_nodeBodyStart + c_nodeRowHeight * row)) });
	}
	for (int32_t i = 0; i < node.GetOutputs().size(); i++, row++) {
		outputs.insert({ &node.GetOutputs()[i], new ShaderNodeOutputObject(*this, node.GetOutputs()[i], Vec2(size.x - 4, c_nodeBodyStart + c_nodeRowHeight * row)) });
	}
}
ShaderNodeObject::~ShaderNodeObject() {
	for (auto pair : inputs) {
		delete pair.second;
	}
	for (auto pair : outputs) {
		delete pair.second;
	}
}

ShaderGraphObject::ShaderGraphObject(ShaderGraph& graph) {
	for (int32_t i = 0; i < graph.GetNodes().size(); i++) {
		m_nodes.insert({ graph.GetNodes()[i], new ShaderNodeObject(*graph.GetNodes()[i], Vec2(100 + 300 * i, 100), GetNodeSize(graph.GetNodes()[i])) });
	}
	for (int32_t i = 0; i < graph.GetConnections().size(); i++) {
		ShaderNodeConnectionObject* connection = new ShaderNodeConnectionObject(
			*graph.GetConnections()[i],
			*m_nodes[&graph.GetConnections()[i]->from.m_parent]->outputs[&graph.GetConnections()[i]->from],
			*m_nodes[&graph.GetConnections()[i]->to.m_parent]->inputs[&graph.GetConnections()[i]->to]
		);
		m_nodes[&graph.GetConnections()[i]->from.m_parent]->outputs[&graph.GetConnections()[i]->from]->connections.push_back(connection);
		m_nodes[&graph.GetConnections()[i]->to.m_parent]->inputs[&graph.GetConnections()[i]->to]->connection = connection;
		m_connections.insert({ graph.GetConnections()[i], connection });
	}
}
ShaderGraphObject::~ShaderGraphObject() {
	for (auto pair : m_nodes) {
		delete pair.second;
	}
	for (auto pair : m_connections) {
		delete pair.second;
	}
}

Vec2 ShaderGraphObject::GetNodeSize(ShaderNode* node) {
	return { 200, c_nodeBodyStart + (node->GetInputs().size() + node->GetOutputs().size()) * c_nodeRowHeight};
}

ShaderGraphWindow::ShaderGraphWindow(PixieEngineApp& app, Interface& inter) :
	InterfaceWindow(app, inter), m_frameBuffer({ 1280, 720 }), m_shaderGraphObject(m_shaderGraph) {
	m_bezierShader = ResourceManager::LoadShader("Bezier2DVertexShader.glsl", "Bezier2DFragmentShader.glsl");
}

void ShaderGraphWindow::Draw() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin((std::string("ShaderGraph##")).c_str())) {
		if (ImGui::IsWindowFocused()) {
			HandleUserInput();
		}

		ImVec2 viewportResolution = ImGui::GetContentRegionAvail();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
		ImVec2 mousePos = ImGui::GetMousePos();
		m_cursorViewportPosition = { mousePos.x - cursorScreenPos.x, mousePos.y - cursorScreenPos.y };
		m_prevCursorPos = m_cursorPos;
		m_cursorPos = m_cursorViewportPosition * m_viewScale + m_viewOffset;
		ImGui::SetNextWindowSize(viewportResolution);
		glm::ivec2 glmViewportResolution = { viewportResolution.x, viewportResolution.y };
		m_frameBuffer.Resize(glmViewportResolution);
		UpdateProjection();

		GLint originalViewport[4];
		glGetIntegerv(GL_VIEWPORT, originalViewport);

		std::shared_ptr<Scene> scene = SceneManager::GetScene();
		Camera camera = Camera(Vec3(-10, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), glm::radians(39.6f), { 1280, 720 }, 0, 100);

		m_shaderGraph.Process(*scene.get(), camera);

		m_frameBuffer.Resize(glmViewportResolution);
		m_frameBuffer.Bind();
		m_frameBuffer.ResizeViewport();
		m_frameBuffer.Clear();

		GLuint id = m_shaderGraph.GetNodes().back()->GetInputTexture("Frame");
		GlobalRenderer::DrawTextureFitted(id, { 1280, 720 }, glmViewportResolution);
		glClear(GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		DrawConnections();
		DrawNodes();
		glEnable(GL_DEPTH_TEST);

		m_frameBuffer.Unbind();

		glViewport(originalViewport[0], originalViewport[1], originalViewport[2], originalViewport[3]);

		ImGui::Image((void*)(uint64_t)m_frameBuffer.m_texture, viewportResolution, { 0.0f, 1.0f }, { 1.0f, 0.0f });
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void ShaderGraphWindow::DrawNodes() {
	for (auto& pair : m_shaderGraphObject.m_nodes) {
		ShaderNodeObject* obj = pair.second;
		GlobalRenderer::DrawUIBox(obj->position, obj->size, obj->baseColor, 5.0f, 0.0f, Vec4(0.0f), m_projection);
		GlobalRenderer::DrawText(obj->node.GetName(), obj->position + Vec2(8.0f, 16.0f), 16.0f, Vec3(1.0f), m_projection);
		for (auto& pair : obj->inputs) {
			ShaderNodeInputObject* in = pair.second;
			GlobalRenderer::DrawUIBox(obj->position + in->offset, { 8, 8 }, Vec4(1.0f, 0.15f, 0.3f, 1.0f), 100.0f, 0.0f, Vec4(0.0f), m_projection);
			GlobalRenderer::DrawText(in->input.m_name, obj->position + in->offset + Vec2(12.0f, 8.0f), 14.0f, Vec3(1.0f), m_projection);
		}
		for (auto& pair : obj->outputs) {
			ShaderNodeOutputObject* out = pair.second;
			GlobalRenderer::DrawUIBox(obj->position + out->offset, { 8, 8 }, Vec4(1.0f, 0.15f, 0.3f, 1.0f), 100.0f, 0.0f, Vec4(0.0f), m_projection);
			GlobalRenderer::DrawText(out->output.m_name, obj->position + out->offset + Vec2(-4.0f, 8.0f), 14.0f, Vec3(1.0f), m_projection, true);
		}
	}
}

void ShaderGraphWindow::DrawConnections() {
	m_bezierShader.Bind();
	m_bezierShader.SetUniformMat4f("mProjection", m_projection);
	for (auto& pair : m_shaderGraphObject.m_connections) {
		pair.second->mesh->Draw();
	}
	if (m_grabbedConnection) {
		m_grabbedConnection->UpdateMesh(m_cursorPos);
		m_grabbedConnection->mesh->Draw();
	}
}

void ShaderGraphWindow::HandleUserInput() {
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
			m_viewScale -= (Float)UserInput::mouseScrollY * 0.1f;
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
					ShaderNodeInputObject* input = pair.second;
					if (!input->connection) continue;
					Vec2 offset = m_cursorPos - node->position;
					if (offset.x > input->offset.x && offset.x < input->offset.x + 8 && offset.y > input->offset.y && offset.y < input->offset.y + 8) {
						m_grabbedConnection = new VirtualShaderNodeConnectionObject(input->connection->from, m_cursorPos);
						RemoveConnection(input->connection);
						m_state = ShaderGraphWindowState::ConnectingNodes;
						grabbedConnection = true;
						break;
					}
				}
				if (grabbedConnection) break;
				for (auto pair : node->outputs) {
					ShaderNodeOutputObject* output = pair.second;
					Vec2 offset = m_cursorPos - node->position;
					if (offset.x > output->offset.x && offset.x < output->offset.x + 8 && offset.y > output->offset.y && offset.y < output->offset.y + 8) {
						m_grabbedConnection = new VirtualShaderNodeConnectionObject(*output, m_cursorPos);
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
			ShaderNodeInputObject* in = pair.second;
			if (in->connection) {
				in->connection->UpdateMesh();
			}
		}
		for (auto& pair : m_grabbedNode->outputs) {
			ShaderNodeOutputObject* out = pair.second;
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
			m_viewScale -= (Float)UserInput::mouseScrollY * 0.1f;
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
					ShaderNodeInputObject* input = pair.second;
					Vec2 offset = m_cursorPos - node->position;
					if (offset.x > input->offset.x && offset.x < input->offset.x + 8 && offset.y > input->offset.y && offset.y < input->offset.y + 8) {
						Connect(m_grabbedConnection->from, *input);
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

void ShaderGraphWindow::UpdateProjection() {
	m_projection = glm::ortho(m_viewOffset.x, m_viewOffset.x + m_frameBuffer.m_resolution.x * m_viewScale, m_viewOffset.y + m_frameBuffer.m_resolution.y * m_viewScale, m_viewOffset.y);
}

void ShaderGraphWindow::Connect(ShaderNodeOutputObject& from, ShaderNodeInputObject& to) {
	ShaderNodeConnection* curerntConnection = to.input.m_connection;
	if (curerntConnection) {
		RemoveConnection(m_shaderGraphObject.m_nodes[&curerntConnection->to.m_parent]->inputs[&curerntConnection->to]->connection);
	}
	ShaderNodeConnection* connection = m_shaderGraph.Connect(&from.output, &to.input);
	ShaderNodeConnectionObject* connectionObject = new ShaderNodeConnectionObject(*connection,
		*m_shaderGraphObject.m_nodes[&connection->from.m_parent]->outputs[&connection->from],
		*m_shaderGraphObject.m_nodes[&connection->to.m_parent]->inputs[&connection->to]);
	m_shaderGraphObject.m_connections[connection] = connectionObject;
	m_shaderGraphObject.m_nodes[&connection->from.m_parent]->outputs[&connection->from]->connections.push_back(connectionObject);
	m_shaderGraphObject.m_nodes[&connection->to.m_parent]->inputs[&connection->to]->connection = connectionObject;
}

void ShaderGraphWindow::RemoveConnection(ShaderNodeConnectionObject* connection) {
	ShaderNodeConnection& c = connection->connection;

	connection->to.connection = nullptr;
	for (int32_t i = 0; i < connection->from.connections.size(); i++) {
		if (connection->from.connections[i] == connection) {
			connection->from.connections.erase(connection->from.connections.begin() + i);
			break;
		}
	}
	delete connection;
	m_shaderGraphObject.m_connections.erase(&c);

	m_shaderGraph.RemoveConnection(&c);
}
